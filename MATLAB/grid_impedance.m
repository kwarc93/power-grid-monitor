function [ deltaV, deltaC ] = grid_impedance( voltage, current, c_threshold )
%GRID_IMPEDANCE Summary of this function goes here
%   Detailed explanation goes here
lv = length(voltage); lc = length(current);
if (lv ~= lc) return;  end;

impedance = 0; deltaV = 0; deltaC = 0;
start_idx = 1;
c_threshold = abs(c_threshold);
c_prev_val = abs(current(1));
num_of_triggers = 0;
phase_diff = 0;

c_mean0 = 0;
v_mean0 = 0;
c_mean1 = 0;
v_mean1 = 0;

state = 0;



for idx = 1:lc

    c_act_val = abs(current(idx));
    
    % State "0"
    if(((c_act_val > (c_prev_val + c_threshold)) || (c_act_val < (c_prev_val - c_threshold)) || (idx == lc)) && (state == 0))
        c_mean0 = mean(current((start_idx):(idx-1)));
        v_mean0 = mean(voltage((start_idx):(idx-1)));
        start_idx = idx;
        state = 1;
        c_prev_val = c_act_val;
        if(num_of_triggers ~= 0)
        num_of_triggers = num_of_triggers + 1;
        phase_diff = angle(v_mean1) - angle(v_mean0);
        deltaV(num_of_triggers) = v_mean1*exp(-phase_diff*1i)-v_mean0;
        deltaC(num_of_triggers) = c_mean1*exp(-phase_diff*1i)-c_mean0;
        end
    

    % State "1"
    elseif(((c_act_val > (c_prev_val + c_threshold)) || (c_act_val < (c_prev_val - c_threshold)) || (idx == lc)) && (state == 1))
        c_mean1 = mean(current((start_idx):(idx-1)));
        v_mean1 = mean(voltage((start_idx):(idx-1)));
        start_idx = idx;
        state = 0;
        c_prev_val = c_act_val;
        num_of_triggers = num_of_triggers + 1;
        phase_diff = angle(v_mean1) - angle(v_mean0);
        deltaV(num_of_triggers) = v_mean1*exp(-phase_diff*1i)-v_mean0;
        deltaC(num_of_triggers) = c_mean1*exp(-phase_diff*1i)-c_mean0;

    else
    c_prev_val = c_act_val;
    end
    

end

