/*
 * dsp_calculations.h
 *
 *  Created on: 09.12.2016
 *      Author: Kwarc
 */

#ifndef __DSP_CALCULATIONS_H_
#define __DSP_CALCULATIONS_H_

#include <stdbool.h>
#include <math.h>
#include "stm32f4xx_hal.h"
#include "arm_math.h"
#include "adc.h"
#include "tim.h"

/* ----------------------------------------------------------------------------
** Macro Defines
** ------------------------------------------------------------------------- */
#define BASE_FREQUENCY			(50U)
#define ADC_SAMPLING_F			(102400UL)//
#define OVERSAMPLING			(16U)//
#define OVERSAMPLING_BITS		(2U)//
#define DECIMATION				(2U)//

#define ADC_BUFFER_LENGTH		(4*ADC_SAMPLING_F/BASE_FREQUENCY)
#define ADC_HALFBUFFER_LENGTH	(ADC_BUFFER_LENGTH/2)
#define ADC_RESOLUTION			(16384U)//
#define DSP_SAMPLING_F			(ADC_SAMPLING_F/(OVERSAMPLING*DECIMATION))
#define FFT_LENGTH				(ADC_HALFBUFFER_LENGTH/(OVERSAMPLING*DECIMATION))
#define RFFT_50HZ_BIN			((BASE_FREQUENCY*FFT_LENGTH)/(DSP_SAMPLING_F))
#define CFFT_50HZ_BIN_R			(2*RFFT_50HZ_BIN)
#define CFFT_50HZ_BIN_I			(2*RFFT_50HZ_BIN + 1)
#define BLOCK_SIZE          	32
#define NUM_TAPS            	29

#define CCMRAM	__attribute__((section (".ccmram")))
#define SQRT2 (1.414213562f)

#define GET_ADC2_RESULT(x)	((uint16_t) ((uint32_t)(x) >> 16))
#define GET_ADC1_RESULT(x)	((uint16_t) ((uint32_t)(x) & 0x0000FFFFUL))
#define GET_HARMONIC(k)		(RFFT_50HZ_BIN*(uint8_t)(k))
#define GET_HARMONIC_R(k)	(CFFT_50HZ_BIN_R*(uint8_t)(k))
#define GET_HARMONIC_I(k)	(CFFT_50HZ_BIN_I*(uint8_t)(k))
#define RMS_VOLTAGE_RANGE	((float32_t)(260.0f * 1.024f))	// [U RMS] * calibr. coeff.
#define RMS_CURRENT_RANGE	((float32_t)(20.0f * 1.1099479f))	// [I RMS] * calibr. coeff.
#define PEAK_VOLTAGE_RANGE	((float32_t)(2.0f*RMS_VOLTAGE_RANGE*SQRT2))	// [U PEAK]
#define PEAK_CURRENT_RANGE	((float32_t)(2.0f*RMS_CURRENT_RANGE*SQRT2))	// [I PEAK]

/* ADC buffer containing result from ADC1(high 16 bits of ADC_CCR) & ADC2(low 16 bits of ADC_CCR)
 * ADC working in Dual-Simultaneous mode with double buffering (DMA half-transfers) */
uint32_t ADC_Buffer[ADC_BUFFER_LENGTH];
uint32_t* ready_buffer;
_Bool DSP_apply_window;
enum DSP_BufferToDisplay {voltage, current, voltageFFT, currentFFT};
typedef enum {ind_load = 1, cap_load, ind_generator, cap_generator} eLoadCharacter;

/* --------------------------------------------------------------------------
** FIR Coefficients buffer generated using fir1() MATLAB function.
** -------------------------------------------------------------------------- */

static const float32_t firCoeffs32[NUM_TAPS] = {	// fir1(28, 1200/1600) for fs = 3200Hz
		0.0018157335f,-0.0015820138f,0.0000000000f,0.0036839755f,-0.0080453466f,0.0084984437f,
		-0.0000000000f,-0.0173328861f,0.0340186559f,-0.0332348824f,0.0000000000f,0.0673788918f,-0.1516391642f,
		0.2220942261f,0.7486887331f,0.2220942261f,-0.1516391642f,0.0673788918f,0.0000000000f,-0.0332348824f,
		0.0340186559f,-0.0173328861f,-0.0000000000f,0.0084984437f,-0.0080453466f,0.0036839755f,0.0000000000f,
		-0.0015820138f,0.0018157335f };

/* --------------------------------------------------------------------
** Hanning window generated using hann(N, 'periodic') MATLAB function.
** -------------------------------------------------------------------- */
static const float32_t hanning[FFT_LENGTH] = {
		0.0000000000f,0.0006022719f,0.0024076367f,0.0054117450f,0.0096073598f,0.0149843734f,
		0.0215298321f,0.0292279674f,0.0380602337f,0.0480053534f,0.0590393678f,0.0711356950f,0.0842651938f,
		0.0983962343f,0.1134947733f,0.1295244373f,0.1464466094f,0.1642205226f,0.1828033579f,0.2021503478f,
		0.2222148835f,0.2429486279f,0.2643016316f,0.2862224533f,0.3086582838f,0.3315550733f,0.3548576614f,
		0.3785099100f,0.4024548390f,0.4266347628f,0.4509914298f,0.4754661628f,0.5000000000f,0.5245338372f,
		0.5490085702f,0.5733652372f,0.5975451610f,0.6214900900f,0.6451423386f,0.6684449267f,0.6913417162f,
		0.7137775467f,0.7356983684f,0.7570513721f,0.7777851165f,0.7978496522f,0.8171966421f,0.8357794774f,
		0.8535533906f,0.8704755627f,0.8865052267f,0.9016037657f,0.9157348062f,0.9288643050f,0.9409606322f,
		0.9519946466f,0.9619397663f,0.9707720326f,0.9784701679f,0.9850156266f,0.9903926402f,0.9945882550f,
		0.9975923633f,0.9993977281f,1.0000000000f,0.9993977281f,0.9975923633f,0.9945882550f,0.9903926402f,
		0.9850156266f,0.9784701679f,0.9707720326f,0.9619397663f,0.9519946466f,0.9409606322f,0.9288643050f,
		0.9157348062f,0.9016037657f,0.8865052267f,0.8704755627f,0.8535533906f,0.8357794774f,0.8171966421f,
		0.7978496522f,0.7777851165f,0.7570513721f,0.7356983684f,0.7137775467f,0.6913417162f,0.6684449267f,
		0.6451423386f,0.6214900900f,0.5975451610f,0.5733652372f,0.5490085702f,0.5245338372f,0.5000000000f,
		0.4754661628f,0.4509914298f,0.4266347628f,0.4024548390f,0.3785099100f,0.3548576614f,0.3315550733f,
		0.3086582838f,0.2862224533f,0.2643016316f,0.2429486279f,0.2222148835f,0.2021503478f,0.1828033579f,
		0.1642205226f,0.1464466094f,0.1295244373f,0.1134947733f,0.0983962343f,0.0842651938f,0.0711356950f,
		0.0590393678f,0.0480053534f,0.0380602337f,0.0292279674f,0.0215298321f,0.0149843734f,0.0096073598f,
		0.0054117450f,0.0024076367f,0.0006022719f };

struct DSP_t
{
	float32_t mean;
	float32_t GRAPH_buffer[FFT_LENGTH];
	float32_t DSP_buffer[FFT_LENGTH];
	float32_t FFT_out_cmplx[FFT_LENGTH];
	float32_t FFT_out_real[FFT_LENGTH/2];
	float32_t FIR_state[BLOCK_SIZE + NUM_TAPS - 1];

	arm_fir_instance_f32 hFIR;
	arm_rfft_fast_instance_f32 hFFT;
};

struct parameters_t
{
	float32_t frequency;
	float32_t RMS_voltage,RMS_current;
	float32_t load_impedance[2];		// complex number
	float32_t grid_impedance;
	float32_t THD_voltage,THD_current;
	float32_t S,P,Q;
	float32_t PF, DPF;
	eLoadCharacter load_type;
	_Bool data_averaged;
}grid;

struct DSP_t U,I;

void DSP_Init(void);
_Bool DSP_AutoselectBuffers(void);
float32_t* DSP_GetBufferPointer(enum DSP_BufferToDisplay type);
void DSP_FIRFilter(void);
void DSP_CalcRMS(void);
void DSP_CalcFrequency(void);
inline void DSP_ADCPLL(void);
void DSP_CalcFFT(void);
void DSP_CalcTHD(void);
void DSP_CalcLoadImpedance(void);
void DSP_GetLoadCharacter(void);
void DSP_CalcDPF(void);
void DSP_CalcPower(void);
void DSP_CalcPF(void);
void DSP_AverageValues(uint8_t avg_number);





#endif /* __DSP_CALCULATIONS_H_ */
