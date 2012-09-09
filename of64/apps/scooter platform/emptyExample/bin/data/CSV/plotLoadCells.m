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

% "zero in the data" - a bit of a cheat
diff0 = loadCell_0(1);
loadCell_0 = loadCell_0 - diff0;
diff1 = loadCell_1(1);
loadCell_1 = loadCell_1 - diff1;
diff2 = loadCell_2(1);
loadCell_2 = loadCell_2 - diff2;
diff3 = loadCell_3(1);
loadCell_3 = loadCell_3 - diff3;

% plot the data
figure(1)
subplot(3,2,1);
plot(clockTime,loadCell_0,'r')
title([titleType, 'Load Cell 0'])
subplot(3,2,3);
plot(clockTime,loadCell_1,'g')
title([titleType, 'Load Cell 1'])
subplot(3,2,4);
plot(clockTime,loadCell_2,'b')
title([titleType, 'Load Cell 2'])
subplot(3,2,2);
plot(clockTime,loadCell_3,'k')
title([titleType, 'Load Cell 3'])
subplot(3,2,[5 6])
hold on
plot(clockTime,loadCell_0,'r')
plot(clockTime,loadCell_1,'g')
plot(clockTime,loadCell_2,'b')
plot(clockTime,loadCell_3,'k')
title([titleType, 'Combined Load Cells'])
hold off
ylabel(yAxis), xlabel('clock time')


figure(2)
plot(clockTime,loadCell_0,'r'),ylabel(yAxis), xlabel('clock time')
title([titleType, 'Load Cell 0'])
figure(3)
plot(clockTime,loadCell_1,'g'),ylabel(yAxis), xlabel('clock time')
title([titleType, 'Load Cell 1'])
figure(4)
plot(clockTime,loadCell_2,'b'),ylabel(yAxis), xlabel('clock time')
title([titleType, 'Load Cell 2'])
figure(5)
plot(clockTime,loadCell_3,'k'),ylabel(yAxis), xlabel('clock time')
title([titleType, 'Load Cell 3'])

appendName = strcat('../Figures/',filename(1:end-4),'-');
saveas(figure(1),strcat(appendName, 'combinedPlot.png'))
saveas(figure(2),strcat(appendName, 'loadCell-0.png'))
saveas(figure(3),strcat(appendName, 'loadCell-1.png'))
saveas(figure(4),strcat(appendName, 'loadCell-2.png'))
saveas(figure(5),strcat(appendName, 'loadCell-3.png'))

end