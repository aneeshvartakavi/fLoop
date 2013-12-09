clear all; close all; clc;

cd('C:\Users\Aneesh\Desktop\Audio\Test\A');
names = dir('*.wav');
names = struct2cell(names);
names = names(1,:)';

numNames = size(names,1);
features = zeros(52,numNames);
for i=1:numNames
    fileName = char(names(i)); 
    
    features(:,i) = featureMFCC(fileName);
    display(i);
end

features1 = features;

cd('C:\Users\Aneesh\Desktop\Audio\Test\B');
names = dir('*.wav');
names = struct2cell(names);
names = names(1,:)';

numNames = size(names,1);
features = zeros(52,numNames);
for i=1:numNames
    fileName = char(names(i)); 
    
    features(:,i) = featureMFCC(fileName);
    display(i);
end

features2 = features;

features = [features1 features2];

features = scaleFeatures(features);

dist = bsxfun(@minus,features,features(:,1));

dist1 = rms(dist);
