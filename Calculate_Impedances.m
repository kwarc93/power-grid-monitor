clear all; close all; clc;
%% SET THESE VALUES !!!
threshold_1 = 0.1;
threshold_2 = 0.001;
ts = 0.25;  %[s]
%% Read .csv file
D = csvread('PGM_Logs\LOG-00A.csv',1,1);
W = csvread('WF00.csv',1);
ADC = importdata('waveforms/rawADC.txt');
% Set desired length of record
len = length(D(:,1));
% sampling time
t_start = 0;
t_stop = ts*(len-1);
time = (t_start:ts:t_stop)';
harmonics = 1:16;
% Write data to apropriate matrixes
frequency = D(1:len,1);
power_factor = D(1:len,2);
S = D(1:len,3); P = D(1:len,4); Q = D(1:len,5);

for col = 1:17
   voltage_R(1:len,col) = D(1:len,col+5);
   voltage_I(1:len,col) = D(1:len,col+22);
   
   current_R(1:len,col) = D(1:len,col+39);
   current_I(1:len,col) = D(1:len,col+56);
end

% combine real and imaginary values of FFT's
voltage_c = voltage_R + 1i*voltage_I;
current_c = current_R + 1i*current_I;
% get absolute value from complex vectors
voltage = abs(voltage_c);
current = abs(current_c);
%% Plot Voltage & Current harmonics (k - harmonic)
k = 1;  %harmonic
k = k+1;
figure;
subplot(211);
plot(time,voltage(:,k)); grid on;
title(sprintf('RMS Voltage (k=%d)', k-1));
xlabel('time [s]');
ylabel('[V]');
subplot(212)
plot(time,current(:,k),'r'); grid on;
title(sprintf('RMS Current (k=%d)', k-1));
xlabel('time [s]');
ylabel('[A]');

%% Plot Power's, Power Factor & Frequency
figure;
subplot(311)
plot(time,frequency); grid on;
title('Frequency');
xlabel('time [s]');
ylabel('[Hz]');
subplot(312)
plot(time,S,time,P,time,Q); grid on;
title('Powers');
xlabel('time [s]');
ylabel('[VA, W, VAr]');
legend('S [VA]','P [W]','Q [Var]');
subplot(313)
plot(time,power_factor,'k'); grid on;
title('Power Factor');
xlabel('time [s]');
%% Calculate load impedance
Z_load = voltage(:,k)./current(:,k);
% Z_load_ind = find(Z_load>50);
% Z_load(Z_load_ind) = nan;
figure;
subplot(211)
plot(time,Z_load); grid on;
title(sprintf('Load impedance (k=%d)', k));
axis([0 time(len) 0 5000]);
xlabel('time [s]');
ylabel('[Ohm]');
%% Calculate source impedance - method 1 (k - harmonic)
for h = 1:17
dI = diff(current(:,h));
dI_thr = threshold_1*max(current(:,h));
peaks_dI = find((dI > dI_thr) | (dI < -dI_thr));
% loop for calculating mean values from selected sections
prev_idx = 1;
meanU = 0; meanI = 0;
for idx = 1:length(peaks_dI)
    meanU(idx) = mean(voltage(prev_idx:peaks_dI(idx),h));
    meanI(idx) = mean(current(prev_idx:peaks_dI(idx),h));
    prev_idx = peaks_dI(idx);
end

diffU = diff(meanU);
diffI = diff(meanI);
if h == k
    Zs_k = -diffU./diffI;
end
Zs = -diffU./diffI;
pos = find(Zs > 0);
Zsh1(h) = mean(Zs(pos));
end
%% Calculate source impedance - method 2
for h = 1:17
    
v = voltage_c(:,h);
c = current_c(:,h);
[ deltaV, deltaI] = grid_impedance(v, c, threshold_2*max(abs(c)));
% Impedance:
Zs2_k = -(sum(conj(deltaI).*deltaV))/(sum(abs(deltaI).^2));
Zsh2(h) =  abs(Zs2_k);
% Coherency:
C(h) = (abs(sum(conj(deltaI).*deltaV)))^2/((sum(abs(deltaI).^2))*(sum(abs(deltaV).^2)));
end
Zs2_k = Zsh2(k) %display
C_k = C(k) %display
%% Plot calculated values
subplot(212)
pos_val = find(Zs_k > 0);
Zs1_k = mean(Zs_k(pos_val)) %display
Zs1_k = Zs1_k*ones(1,length(pos_val));
plot(Zs_k(pos_val),'-o'); grid on; hold on;
plot(Zs1_k, 'r');
plot(Zsh2(k)*ones(1,length(pos_val)), 'g');
title(sprintf('Source impedance (k=%d)', k));
xlabel('number of load change detections');
ylabel('[Ohm]');
legend('[1]',sprintf('[1](mean): |Z|=%.2f Ohm', Zs1_k(1)),sprintf('[2]: |Z|=%.2f Ohm', Zsh2(k)));
legend('Location','best');


%% Plot harmonic impedance
figure;
subplot(211);
plot(harmonics, Zsh1(2:end), '-o'); grid on;
title('Harmonic impedance of power grid (method 1)');
xlabel('harmonic');
ylabel('[Ohm]');
subplot(212);
plot(harmonics, Zsh2(2:end), '-or'); grid on;
title('Harmonic impedance of power grid (method 2)');
xlabel('harmonic');
ylabel('[Ohm]')

%% Plot waveforms
% figure;
% subplot(211)
% plot(W(:,1)); grid on;
% title('Voltage');
% xlabel('samples');
% ylabel('[V]');
% subplot(212);
% plot(W(:,2),'r'); grid on;
% title('Current');
% xlabel('samples');
% ylabel('[A]');
% figure;
% plot(ADC.data(:,1)); grid on; hold on;
% plot(ADC.data(:,2),'--r');