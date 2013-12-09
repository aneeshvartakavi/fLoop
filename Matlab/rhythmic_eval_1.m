clear all; close all; clc;

test = computeBeatSpectrumFeatures('04 - Hats - 110bpm.wav','empty');

names = [dir('*.wav') ; dir('*.mp3')];

names = struct2cell(names);
names = names(1,:)';

numNames = size(names,1);

tempFeatures = zeros(16,length(numNames));

for j=1:numNames
    fileName = char(names(j));
    tempFeatures(:,j) = computeBeatSpectrumFeatures(fileName,'empty');
end

allFeatures = [test tempFeatures];

% Normalization

for i=1:size(allFeatures,1)-1
    allFeatures(i,:) = (allFeatures(i,:)-mean(allFeatures(i,:)))/std(allFeatures(i,:)); 
end

dist = bsxfun(@minus,allFeatures,allFeatures(:,1));

dist1 = rms(dist);