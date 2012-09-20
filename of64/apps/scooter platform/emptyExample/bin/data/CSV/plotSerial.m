function plotSerial(filename)

% close all existing figures & clear workspace
close all

% load in the csv file
inputMat = csvread(filename);

serial = inputMat(1:86,1);
clockTime = inputMat(1:86,2);

plot(clockTime,serial,'x-')
title('Potentiometer Value vs. Time[s]')
ylabel('raw potentiometer value'), xlabel('time [s]')

appendName = strcat('../Figures/',filename(1:end-4));
saveas(figure(1),strcat(appendName, '.png'))

end