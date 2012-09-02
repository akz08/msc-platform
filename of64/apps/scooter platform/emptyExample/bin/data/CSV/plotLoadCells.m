function plotLoadCells(filename)

% close all existing figures & clear workspace
close all

% load in the csv file
inputMat = csvread(filename);

% extract cal/raw from filename
dataType = filename(end-14:end-12);

% to choose which label, title, and y axis
if strcmp(dataType,'cal') 
    yAxis = 'Voltage [mv]';
    titleType = 'Calibrated ';
end

if strcmp(dataType,'raw')
    yAxis = 'Weight [kg]';
    titleType = 'Raw ';
end

loadCell_0 = inputMat(:,1);
loadCell_1 = inputMat(:,2);
loadCell_2 = inputMat(:,3);
loadCell_3 = inputMat(:,4);

clockTime = inputMat(:,5);

% plot the data
figure(1)
subplot(3,2,1);
plot(loadCell_0,clockTime,'r')
title([titleType, 'Load Cell 0'])
subplot(3,2,2);
plot(loadCell_1,clockTime,'g')
title([titleType, 'Load Cell 1'])
subplot(3,2,3);
plot(loadCell_2,clockTime,'b')
title([titleType, 'Load Cell 2'])
subplot(3,2,4);
plot(loadCell_3,clockTime,'k')
title([titleType, 'Load Cell 3'])
subplot(3,2,[5 6])
hold on
plot(loadCell_0,clockTime,'r')
plot(loadCell_1,clockTime,'g')
plot(loadCell_2,clockTime,'b')
plot(loadCell_3,clockTime,'k')
title([titleType, 'Combined Load Cells'])
hold off
ylabel(yAxis), xlabel('clock time')


figure(2)
plot(loadCell_0,clockTime,'r'),ylabel(yAxis), xlabel('clock time')
title([titleType, 'Load Cell 0'])
figure(3)
plot(loadCell_1,clockTime,'g'),ylabel(yAxis), xlabel('clock time')
title([titleType, 'Load Cell 1'])
figure(4)
plot(loadCell_2,clockTime,'b'),ylabel(yAxis), xlabel('clock time')
title([titleType, 'Load Cell 2'])
figure(5)
plot(loadCell_3,clockTime,'k'),ylabel(yAxis), xlabel('clock time')
title([titleType, 'Load Cell 3'])

appendName = strcat('../Figures/',filename(1:end-4),'-');
saveas(figure(1),strcat(appendName, 'combinedPlot.png'))
saveas(figure(2),strcat(appendName, 'loadCell-0.png'))
saveas(figure(3),strcat(appendName, 'loadCell-1.png'))
saveas(figure(4),strcat(appendName, 'loadCell-2.png'))
saveas(figure(5),strcat(appendName, 'loadCell-3.png'))

end