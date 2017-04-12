/*
 * dsp_calculations.c
 *
 *  Created on: 09.12.2016
 *      Author: Kwarc
 */

#include "dsp_calculations.h"
#include "pga.h"
#include "cmsis_os.h"
#include "osThreads.h"
#include <stdbool.h>

static volatile uint8_t DMA_Half_Ready = 0;
static volatile uint8_t DMA_Full_Ready = 0;

static uint32_t blockSize = BLOCK_SIZE;
static uint32_t numBlocks = FFT_LENGTH/BLOCK_SIZE;

/* ADC buffer containing result from ADC1(high 16 bits of ADC_CCR) & ADC2(low 16 bits of ADC_CCR)
 * ADC working in Dual-Simultaneous mode with double buffering (DMA half-transfers) */
static uint32_t ADC_Buffer[ADC_BUFFER_LENGTH];

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	// When DMA(ADC1 & ADC2) completes first-half transfer to ADC buffer:
	DMA_Half_Ready = 1;
	osSemaphoreRelease(DSP_Semaphore);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	// When DMA(ADC1 & ADC2) completes second-half transfer to ADC buffer:
	DMA_Full_Ready = 1;
	osSemaphoreRelease(DSP_Semaphore);
}

void DSP_Init(void)
{

	/* Call FIR init function to initialize the instance structure. */
	arm_fir_init_f32(&U.hFIR, NUM_TAPS, (float32_t *)&firCoeffs32[0], &U.FIR_state[0], blockSize);
	arm_fir_init_f32(&I.hFIR, NUM_TAPS, (float32_t *)&firCoeffs32[0], &I.FIR_state[0], blockSize);

	/* Call RFFT init function to initialize the instance structure. */
	arm_rfft_fast_init_f32(&U.hFFT, FFT_LENGTH);
	arm_rfft_fast_init_f32(&I.hFFT, FFT_LENGTH);

	/* Start timer2 for ADC triggering */
	HAL_TIM_Base_Start_IT(&htim2);
	/* Start timer3 for frequency measuring */
	HAL_TIM_IC_Start(&htim3,TIM_CHANNEL_1);
	/* Start ADC1(master) & ADC2 in dual, double-buffering mode for simultaneous signals acquisition */
	HAL_ADCEx_MultiModeStart_DMA(&hadc1, ADC_Buffer, ADC_BUFFER_LENGTH);
	HAL_ADC_Start_IT(&hadc2);

	PGA_SetGain(x1);
	grid.data_averaged = false;
}

/* ----------------------------------------------------------------------
 ** Check which memory (Memory0 or Memory1) is busy now,
 ** based on DMA_SxCR_CT bit, and select active buffer for DSP calculations
 ** ------------------------------------------------------------------- */
_Bool DSP_AutoselectBuffers(void)
{
	volatile uint32_t U_mean, I_mean;
	int32_t U_temporary, I_temporary;
	/* Signal buffer selecting */
	if(DMA_Half_Ready || DMA_Full_Ready)
	{
		DSP_ADCPLL();

		if(DMA_Half_Ready)
		{
			ready_buffer = &ADC_Buffer[0];
			DMA_Half_Ready = 0;
		}
		if(DMA_Full_Ready)
		{
			ready_buffer = &ADC_Buffer[ADC_HALFBUFFER_LENGTH];
			DMA_Full_Ready = 0;
		}


		// Calculate mean value
		U_mean = I_mean = 0;
		for(uint16_t i = 0; i < ADC_HALFBUFFER_LENGTH; i++)
		{
			U_mean += GET_ADC1_RESULT(ready_buffer[i]);
			I_mean += GET_ADC2_RESULT(ready_buffer[i]);
		}
		U_mean /= ADC_HALFBUFFER_LENGTH; I_mean /= ADC_HALFBUFFER_LENGTH;

		// Enhance resolution, delete DC offset and convert uint32_t to float32_t for CMSIS-DSP purposes:
		/********************** OVERSAMPLING & DECIMATION *********************/
		for(uint16_t idx = 0;idx < ADC_HALFBUFFER_LENGTH; idx += OVERSAMPLING)
		{
			U_temporary = I_temporary = 0;
			for(uint16_t sample = idx;sample < (idx + OVERSAMPLING); sample++)
			{
				U_temporary += (GET_ADC1_RESULT(ready_buffer[sample]) - U_mean);
				I_temporary += (GET_ADC2_RESULT(ready_buffer[sample]) - I_mean);
			}
			U_temporary >>= OVERSAMPLING_BITS; I_temporary >>= OVERSAMPLING_BITS;
			U.DSP_buffer[idx/(OVERSAMPLING*DECIMATION)] = (float32_t)U_temporary;
			I.DSP_buffer[idx/(OVERSAMPLING*DECIMATION)] = (float32_t)I_temporary;
		}
		/*********************************************************************/
		return true;
	}
	else	return false;
}
/* ---------------------------------------------------------------------------
 * Return a pointer to selected buffer
 * --------------------------------------------------------------------------- */
inline float32_t* DSP_GetBufferPointer(enum DSP_BufferToDisplay type)
{
	float32_t* buff_array[4] = {U.GRAPH_buffer, I.GRAPH_buffer, U.FFT_out_real, I.FFT_out_real};

	return buff_array[type];
}

/* ----------------------------------------------------------------------
 ** Call the FIR process function for every blockSize samples
 ** ------------------------------------------------------------------- */
void DSP_FIRFilter(void)
{
	for(uint16_t i=0; i < numBlocks; i++)
	{
		arm_fir_f32(&U.hFIR, U.DSP_buffer + (i * blockSize), U.DSP_buffer + (i * blockSize), blockSize);
		arm_fir_f32(&I.hFIR, I.DSP_buffer + (i * blockSize), I.DSP_buffer + (i * blockSize), blockSize);
	}
}
/* ----------------------------------------------------------------------
 ** Compute the RMS value of measured signals: voltage & current
 ** ------------------------------------------------------------------- */
void DSP_CalcRMS(void)
{
	arm_rms_f32(U.DSP_buffer, FFT_LENGTH, &grid.RMS_voltage);
	grid.RMS_voltage = (2.0f*SQRT2*RMS_VOLTAGE_RANGE*grid.RMS_voltage)/((float32_t)ADC_RESOLUTION);

	arm_rms_f32(I.DSP_buffer, FFT_LENGTH, &grid.RMS_current);
	grid.RMS_current = (2.0f*SQRT2*RMS_CURRENT_RANGE*grid.RMS_current)/((float32_t)ADC_RESOLUTION*pga_gain);
}

/* ----------------------------------------------------------------------
 ** Compute the voltage frequency
 ** ------------------------------------------------------------------- */
void DSP_CalcFrequency(void)
{
	uint32_t signal_period;

	signal_period = __HAL_TIM_GET_COMPARE(&htim3,TIM_CHANNEL_1);
	grid.frequency = (float32_t)((2000000.0F)/signal_period);

}
/* ----------------------------------------------------------------------
 ** Correct ADC triggering depending on measured frequency
 ** ------------------------------------------------------------------- */
inline void DSP_ADCPLL(void)
{
	if(grid.data_averaged == true && grid.frequency > 45.0F && grid.frequency < 55.0F)
	{
		/* Set new value of TIM2 ARR period for correct timing of ADC triggering */
		htim2.Instance->ARR = (uint32_t)(29325.5F/grid.frequency) - 1;
	}
}

/* -----------------------------------------------------------------------
** Calculate FFT for voltage & current
** -------------------------------------------------------------------- */
void DSP_CalcFFT(void)
{
	/* Apply window on signal if window flag is true */
	if(DSP_apply_window)
	{
		arm_mult_f32(U.DSP_buffer, (float32_t*)hanning, U.DSP_buffer, FFT_LENGTH);
		arm_mult_f32(I.DSP_buffer, (float32_t*)hanning, I.DSP_buffer, FFT_LENGTH);
	}
	/* Process the data through the RFFT/RIFFT module */
	arm_rfft_fast_f32(&U.hFFT,U.DSP_buffer,U.FFT_out_cmplx,0);
	arm_rfft_fast_f32(&I.hFFT,I.DSP_buffer,I.FFT_out_cmplx,0);

	/* Scale complex FFT output */
	arm_scale_f32(U.FFT_out_cmplx, (DSP_apply_window ? 2.0f*RMS_VOLTAGE_RANGE : RMS_VOLTAGE_RANGE)*4.0f/((float32_t)FFT_LENGTH*ADC_RESOLUTION), U.FFT_out_cmplx, FFT_LENGTH);
	arm_scale_f32(I.FFT_out_cmplx, (DSP_apply_window ? 2.0f*RMS_CURRENT_RANGE : RMS_CURRENT_RANGE)*4.0f/((float32_t)FFT_LENGTH*ADC_RESOLUTION*pga_gain), I.FFT_out_cmplx, FFT_LENGTH);

	/* Process the data through the Complex Magnitude Module for
		calculating the magnitude at each bin */
	arm_cmplx_mag_f32(U.FFT_out_cmplx, U.FFT_out_real, FFT_LENGTH/2);
	arm_cmplx_mag_f32(I.FFT_out_cmplx, I.FFT_out_real, FFT_LENGTH/2);

}
/* ----------------------------------------------------------------------
 ** Calculate angle between voltage & current (for fundamental freq.)
 ** WARNING - Function must be called after DSP_CalcFFT() !
 ** ------------------------------------------------------------------- */
void DSP_CalcDPF(void)
{
	grid.DPF = (float32_t)(atan2f(U.FFT_out_cmplx[CFFT_50HZ_BIN_I], U.FFT_out_cmplx[CFFT_50HZ_BIN_R]) -
							 atan2f(I.FFT_out_cmplx[CFFT_50HZ_BIN_I], I.FFT_out_cmplx[CFFT_50HZ_BIN_R]));
	grid.DPF = arm_cos_f32(grid.DPF);
}
/* ----------------------------------------------------------------------
 ** Calculate THD of voltage & current referred to fundamental 50Hz
 ** WARNING - Function must be called after DSP_CalcFFT() !
 ** ------------------------------------------------------------------- */
void DSP_CalcTHD(void)
{
	float32_t U_sumTHD, I_sumTHD, result;
	uint16_t idx;

	U_sumTHD = I_sumTHD = 0;

	for(uint16_t i = 0; i < FFT_LENGTH/2 - (2*RFFT_50HZ_BIN); i += RFFT_50HZ_BIN)
	{
		idx = 2*RFFT_50HZ_BIN + i;
		U_sumTHD += U.FFT_out_real[idx]*U.FFT_out_real[idx];
		I_sumTHD += I.FFT_out_real[idx]*I.FFT_out_real[idx];
	}

	arm_sqrt_f32(U_sumTHD, &result);
	grid.THD_voltage = (float32_t)(100*result)/U.FFT_out_real[RFFT_50HZ_BIN];
	arm_sqrt_f32(I_sumTHD, &result);
	grid.THD_current = (float32_t)(100*result)/I.FFT_out_real[RFFT_50HZ_BIN];
}

/* ----------------------------------------------------------------------
 ** Calculate apparent, active and reactive power
 ** WRANING - Function must be called after DSP_CalcRMS() & DSP_CalcFFT() !
 ** ------------------------------------------------------------------- */
void DSP_CalcPower(void)
{
	float32_t realResult, imagResult;

	grid.S = grid.RMS_voltage*grid.RMS_current;

	/* Dot product of FFT complex outputs voltage & current */
	realResult=0;
	imagResult=0;

	for(uint16_t n = 0; n < (FFT_LENGTH/2); n+=CFFT_50HZ_BIN_R)
	{
		realResult += U.FFT_out_cmplx[n+0]*I.FFT_out_cmplx[n+0] +
					  U.FFT_out_cmplx[n+1]*I.FFT_out_cmplx[n+1];
		imagResult += I.FFT_out_cmplx[n+0]*U.FFT_out_cmplx[n+1] -
					  I.FFT_out_cmplx[n+1]*U.FFT_out_cmplx[n+0];
	}

	grid.P = realResult;
	grid.Q = -imagResult;
}
/* ----------------------------------------------------------------------
 ** Calculate power factor
 ** WRANING - Function must be called after DSP_CalcPower() !
 ** ------------------------------------------------------------------- */
void DSP_CalcPF(void)
{
	grid.PF = grid.P/grid.S;
	if(grid.PF > 1.0f) grid.PF = 1.0f;
}
/* ----------------------------------------------------------------------
 ** Calculate load impedance
 ** WRANING - Function must be called after DSP_CalcPower() & DSP_CalcPF() !
 ** ------------------------------------------------------------------- */
void DSP_CalcLoadImpedance(void)
{
	float32_t res;
	/* Load impedance from RMS values*/
	if(grid.RMS_current > 0.0)
	{
		res =  grid.RMS_voltage/grid.RMS_current;
		// Real part
		grid.load_impedance[0] = res*grid.PF;
		// Imaginary part
		grid.load_impedance[1] = ((grid.Q > 0.0f) ? 1.0f : -1.0f)*arm_sin_f32(acosf(grid.PF))*res;
	}
}
/* ----------------------------------------------------------------------
 ** Get load type (inductive/capacitive load/generator)
 ** WRANING - Function must be called after DSP_CalcPower() & DSP_CalcPF() !
 ** ------------------------------------------------------------------- */
void DSP_GetLoadCharacter(void)
{
	if(grid.P >= 0.0f && grid.Q >= 0.0f)		{grid.load_type = ind_load;}
	else if(grid.P < 0.0f && grid.Q >= 0.0f)	{grid.load_type =  ind_generator;}
	else if(grid.P < 0.0f && grid.Q < 0.0f)		{grid.load_type =  cap_generator;}
	else if(grid.P >= 0.0f && grid.Q < 0.0f)	{grid.load_type =  cap_load;}
}
/* ----------------------------------------------------------------------
 ** Average all parameters in grid structure
 ** ------------------------------------------------------------------- */
void DSP_AverageValues(uint8_t avg_number)
{
	static struct parameters_t accum = {0};
	static uint8_t counter = 0;

	if(grid.data_averaged == true) grid.data_averaged = false;

	/* Average some parameters */
	if(counter < avg_number)
	{
		accum.RMS_voltage += grid.RMS_voltage;
		accum.RMS_current += grid.RMS_current;
		accum.frequency += grid.frequency;
		accum.S += grid.S;
		accum.P += grid.P;
		accum.Q += grid.Q;
		accum.PF += grid.PF;
		accum.DPF += grid.DPF;
		accum.load_impedance[0] += grid.load_impedance[0];
		accum.load_impedance[1] += grid.load_impedance[1];
		accum.THD_voltage += grid.THD_voltage;
		accum.THD_current += grid.THD_current;
		counter++;
	}
	else
	{
		float32_t div = 1.0f/(float32_t)avg_number;
		/* Copy averaged parameters to its original structure */
		grid.RMS_voltage = accum.RMS_voltage*div;
		grid.RMS_current = accum.RMS_current*div;
		grid.frequency = accum.frequency*div;
		grid.S = accum.S*div;
		grid.P = accum.P*div;
		grid.Q = accum.Q*div;
		grid.PF = accum.PF*div;
		grid.DPF = accum.DPF*div;
		grid.THD_voltage = accum.THD_voltage*div;
		grid.THD_current = accum.THD_current*div;
		grid.load_impedance[0] = accum.load_impedance[0]*div;
		grid.load_impedance[1] = accum.load_impedance[1]*div;

		/* Indicate that averaging is done, then reset accumulators and counter */
		grid.data_averaged = true;
		accum.S = accum.P = accum.Q = accum.PF = accum.load_impedance[0] = accum.load_impedance[1] = 0;
		accum.RMS_voltage = accum.RMS_current = accum.frequency = accum.DPF = 0;
		accum.THD_voltage = accum.THD_current = 0;
		counter = 0;
	}
}
