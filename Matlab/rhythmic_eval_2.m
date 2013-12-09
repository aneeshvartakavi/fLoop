clear all; close all; clc;

loopDir = 'C:\Users\Aneesh\Desktop\Audio\Testing\';

classes = [1 1 2 2 3 3 4 4 5 5 6 6 7 7 8 8 9 9 10 10];

names = dir(strcat(loopDir, '*.wav'));
names = struct2cell(names);
names = names(1,:)';

numNames = size(names,1);
features = zeros(16,numNames);
for i=1:numNames
    fileName = char(names(i)) 
    
    features(:,i) = computeBeatSpectrumFeatures(strcat(loopDir, fileName), fileName);
%     display(i);
end


for i=1:size(features, 1)
   
    m = mean(features(i, :));
    sd = std(features(i, :));
        
    features(i, :) = (features(i, :) - m) / sd;
    
end


accuracies = [];
for i=1:numNames    
   dists = zeros(20, 1);
   char(names(i))

    for j=1:numNames
        
       X = [features(:, i)'; features(:, j)'];
       dists(j) = pdist(X);
              
    end
    
   dists = [dists classes'];
   
   dists = sortrows(dists, 1);
   
   accuracies = [accuracies mean(dists(2:2, 2)==classes(i))];
   
   figure;
    
    gscatter(1:20, dists(:, 1), dists(:, 2))
    title(strcat('Spectral Periodicity Feature Vector Distance: ', char(names(i))))
    xlabel('Audio Loops')
    ylabel('Distance')
    legend('Location', 'NorthWest')
    
end

mean(accuracies)

