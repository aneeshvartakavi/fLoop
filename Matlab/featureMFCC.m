function [ mfcc_features ] = featureMFCC(fileName)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

blockSize = 1024;
hopSize = 512;
     
[samples,Fs] = audioread(fileName);

samples = samples/max(abs(samples));
blocks = buffer(samples,blockSize,blockSize-hopSize);

% Compute magnitude spectrum
spec = fft(blocks,blockSize);
spec((blockSize/2)+2:end,:) = [];
spec = abs(spec);

S = spec;

periodogram = (S.^2)./size(S,1);

fLower = 300;
fUpper = 8000;
n = 13;

% Convert the frequencies into mels
fLower_mels = 1125*log(1+(fLower/700));
fUpper_mels = 1125*log(1+(fUpper/700));

% Find 2*n equally spaced points in mel scale
bands_mels = linspace(fLower_mels,fUpper_mels,2*n); 
dist = bands_mels(2) - bands_mels(1);
% Add extra bands
bands_mels(end+1) = bands_mels(end) + dist;
bands_mels(end+1) = bands_mels(end) + dist;

% Convert into Hz
bands_hz = 700*(exp(bands_mels./1125)-1);

% Find the bin indices
ind_bin = floor(bands_hz*2*size(S,1)/Fs);

% Creating the triangle windows
filters = zeros(size(S,1),length(ind_bin));
for i=1:length(ind_bin)-2
    
    L = ind_bin(i+2)-ind_bin(i);
    % Create a triangle window
    tempWindow = triang(L);
    
    % Shift it into position
    filters(ind_bin(i):ind_bin(i+2)-1,i) = tempWindow;
    
end

% Apply the filter and compute energy
%energy = zeros(2*n,size(S,2));
%for i=1:size(S,2)
%    energy(:,i) = filters'*periodogram(:,i);
%end

energy = filters'*periodogram;

% Prevent NaN errors for log(0)
energy(energy==0) = 1e-10;
% Find log of the energies
energy_log = log(energy);

% Find the dct
feature = dct(energy_log);

% Keep only the first n
feature(n+1:end,:) = [];

feature_d = [feature(:,1) feature];
feature_d = diff(feature_d,1,2);

mfcc_features = zeros(52,1);
mfcc_features(1:13) = mean(feature,2);
mfcc_features(14:26) = std(feature,0,2);
mfcc_features(27:39) = mean(feature_d,2);
mfcc_features(40:52) = std(feature_d,0,2);


end