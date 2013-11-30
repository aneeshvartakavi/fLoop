function [ features ] = computeBeatSpectrumFeatures( fileName )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

blockSize = 1024;
hopSize = 512;

features = zeros(16,1);
% Not sure if KissFFT uses a window
% W = diag(sparse(hann(blockSize)));

[samples,Fs] = audioread(fileName);
samples = samples/max(abs(samples));
blocks = buffer(samples,blockSize,blockSize-hopSize);

% Compute magnitude spectrum
spec = fft(blocks,blockSize);
spec((blockSize/2)+2:end,:) = [];
spec = abs(spec);

% The paper mentions taking the log, not implemented in c++ yet
spec = log(spec+realmin);

% Find the distance matrix
distances = pdist2(spec',spec','cosine');
% Match the similarity measure to the one in the paper
distances = 1-distances;

colSums = zeros(size(distances,2),1);
for i1=1:size(spec,2)
        colSums(i1) = sum(diag(distances,i1));
end
  
% Remove line of best fit in least squared sense
x=1:length(colSums);
x = x';

meanX = mean(x);
meanY = mean(colSums);
% sum of squares of x values
sumX = sum(x.^2);

% Cumpute the prod of sums
sumXY = sum(x.*colSums);

slope = (sumXY - ((sum(x)*sum(colSums))/length(colSums)))/(sumX - (sum(x))^2/length(colSums));

b = meanY - slope*meanX;

features(1) = slope;
features(2) = b;
% Create the vector to subtract
trend = slope*x + b;

deTrend = colSums - trend;

% Normalize

deTrend = deTrend/max(abs(deTrend));

% Estimate tempo

tempo = 2 * 480 * Fs / length(samples);

% Energy per beat
%tempo = 82; % bpm

firstBeat = tempo;
firstBeatTime = 60/firstBeat; % time in seconds

index = ceil((Fs*firstBeatTime - blockSize/2)/hopSize);

% Find BS values at the beat.
instBeat = [ 0 0 0 0];

for i=0:4:12
    instBeat(1) = instBeat(1) + deTrend((i)*index + 1);
    instBeat(2) = instBeat(2) + deTrend((i+1)*index+ 1);
    instBeat(3) = instBeat(3) + deTrend((i+2)*index+ 1);
    instBeat(4) = instBeat(4) + deTrend((i+3)*index + 1);
end
instBeat = instBeat./sum(abs(instBeat));

features(3:6) = instBeat;
% Energy per beat
enBeat = [0 0 0 0];
for i=0:4:12
    enBeat(1) = enBeat(1) + sum(deTrend((i)*index + 1 : (i+1)*index+ 1));
    enBeat(2) = enBeat(2) + sum(deTrend((i+1)*index + 1 : (i+2)*index+ 1));
    enBeat(3) = enBeat(3) + sum(deTrend((i+2)*index + 1 : (i+3)*index+ 1));
    enBeat(4) = enBeat(4) + sum(deTrend((i+4)*index + 1 : (i+4)*index+ 1));
end

enBeat = enBeat./sum(abs(enBeat));

features(6:9) = enBeat;

bsLen = length(deTrend);

bsMean = mean(deTrend);
features(10) = bsMean;

bsStd = std(deTrend);
features(11) = bsStd;

%=========Skewness

tmp = 0;
for i=1:bsLen    
   tmp = tmp + (deTrend(i) - bsMean) .^ 3;
end

bsSkewness = tmp / (bsStd.^3 * bsLen);
features(12) = bsSkewness;

%==========Kurtosis

tmp = 0;
for i=1:bsLen
   tmp = tmp + (deTrend(i) - bsMean) .^ 4;     
end

bsKurtosis = (tmp / bsLen) / (bsStd.^4) - 3;
features(13) = bsKurtosis;

%====First peak to Second peak ratio
tmp = 0;
for i=2:bsLen
    slope = deTrend(i) - deTrend(i-1);
    
    if slope > 0        
        peak2 = max(deTrend(i:bsLen));        
        
        ppRatio = deTrend(1) / peak2;
        break
    end 
end

features(14) = ppRatio;

%====Cummulative difference b/t BS and straight line through signal
lineSlope = (deTrend(bsLen) - deTrend(1))/bsLen;
yIntercept = deTrend(1);

cumDiff = 0;
for i=1:bsLen
    refPt = lineSlope * i + yIntercept;    
    cumDiff = cumDiff + abs(deTrend(i) - refPt);    
end

features(15) = cumDiff;


%====Num changes in slope
numSlopeChanges = 0;
slopeSign = -1;

for i=2:bsLen
    slope = deTrend(i) - deTrend(i-1);
            
    if sign(slope) == slopeSign
        
    else
        slopeSign = sign(slope);
        numSlopeChanges = numSlopeChanges + 1;
    end    
end

features(16) = numSlopeChanges;

end

