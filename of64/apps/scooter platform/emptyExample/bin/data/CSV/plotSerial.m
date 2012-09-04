function plotSerial(filename)

% close all existing figures & clear workspace
close all

% load in the csv file
inputMat = csvread(filename);

serial = inputMat(:,1);
clockTime = inputMat(:,2);

plot(serial,clockTime)
title('Potentiometer Value vs. Clock Time')
ylabel('raw potentiometer value'), xlabel('clock time')

appendName = strcat('../Figures/',filename(1:end-4));
saveas(figure(1),strcat(appendName, '.png'))

end