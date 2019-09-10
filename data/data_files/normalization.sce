filename = "sample00_square_00.csv";
N = csvread(filename);

meanN = mean(N);

stan_var = std(N);

normX = N(:,1) - meanN(1);
normY = N(:,2) - meanN(2);
normZ = N(:,3) - meanN(3);

normX = normX ./ stan_var(1);
normY = normY ./ stan_var(2);
normZ = normZ ./ stan_var(3);

norm = [normX normY normZ];

csvwrite(filename,norm);
