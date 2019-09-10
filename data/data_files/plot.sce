filename = "CompassoQuaternario-3d.csv";
cd /home/luiz/Documents/projects/maestro/data
N = csvRead(filename);
scatter3(N(:,1),N(:,2),N(:,3),".")
//scatter(N(:,1),N(:,2),".")
