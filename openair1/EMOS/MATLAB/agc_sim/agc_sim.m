% this script simulates the agc in cbmimo1 cards

nFrames = 1e4;

coef = 0.5;  % filter coefficient
hist = 10;   % hysterisis
step = 1;    % step size of agc in dB

TARGET_RX_POWER = 50; % in dB
TX_POWER_START = 70;

gain_test = 0;
gain_agc = 0;
direction = 0;

TX_POWER = zeros(1,nFrames);
RX_POWER = zeros(1,nFrames);
rx_power_fil = zeros(1,nFrames);

for frame = 1:nFrames
    % Transmitter
    if (mod(frame,100) == 0)
        if (gain_test >= 0)
            direction = -1;
        elseif (gain_test <=-40)
            direction = 1;
        end
        %else don't change direction
        gain_test = gain_test + direction;
    end
    TX_POWER(frame) = TX_POWER_START + gain_test + randn(1);

    % Receiver
    RX_POWER(frame) = TX_POWER(frame) + gain_agc;

    % Filter rx_power to reduce measurement noise
    if (frame == 1)
        rx_power_fil(frame) = RX_POWER(frame);
    else
        rx_power_fil(frame) = (rx_power_fil(frame-1) * coef) + (RX_POWER(frame) * (1-coef));
    end

    % Gain control with hysterisis
    if ( (rx_power_fil(frame) < TARGET_RX_POWER - hist/2) )
        gain_agc = gain_agc + step;
    elseif ( (rx_power_fil(frame) > TARGET_RX_POWER + hist/2) )
        gain_agc = gain_agc - step;
    end

    %RX_POWER(frame) = RX_POWER(frame) + gain_agc;
end

%% plot
figure(1)
hold off
plot(TX_POWER)
hold on
plot(rx_power_fil,'g')
plot(RX_POWER,'r')
