filename = "sample01_square_01.csv";
N = csvread(filename);

Z = zeros(8, 3);
i = 1;

for n = 1:1566
    if (mod(n,113) == 0)
        Z(i,:) = N(n,:);
        i=i+1;
    end
end

csvwrite("sample01_square_01_amo1.csv",Z);