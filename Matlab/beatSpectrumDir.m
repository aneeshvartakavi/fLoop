% Compute analysis for directory

clear all; close all; clc;

names = dir('*.wav');
names = struct2cell(names);
names = names(1,:)';

numNames = size(names,1);
features = zeros(16,numNames);
for i=1:numNames
    fileName = char(names(i)); 
    
    features(:,i) = computeBeatSpectrumFeatures(fileName);
    display(i);
end

% Normalize features, remove means across all rows

% Append numeric labels to the last row

% Perform crossvalidation
