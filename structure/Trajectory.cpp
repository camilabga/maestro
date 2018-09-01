#include "Trajectory.h"

Trajectory::Trajectory(string file){
    getPointsFromCSV(file);
    to_send = (char*) malloc(sizeof(char) * 1024);
	temp0 = (char*) malloc(sizeof(char) * 1024);
    temp1 = (char*) malloc(sizeof(char) * 1024); 

	index_s=0;
}

Trajectory::Trajectory(){
    to_send = (char*) malloc(sizeof(char) * 1024);
	temp0 = (char*) malloc(sizeof(char) * 1024);
    temp1 = (char*) malloc(sizeof(char) * 1024); 

	index_s=0;
}

Trajectory::~Trajectory(){

}

void Trajectory::getPointsFromCSV(string file){
    ifstream myfile(file);
	//char temp0[10], temp1[10], temp2[10];
	char temp1[10], temp2[10];
	x.clear();
	y.clear();
	if (myfile.is_open()){
		while (myfile.good()) {
			//myfile.getline (temp0, 10, ',' );
			myfile.getline (temp1, 10, ',' );
			myfile.getline (temp2, 10);
			x.push_back(atof(temp1));
			y.push_back(atof(temp2));
		}

		myfile.close();
	}
}

void Trajectory::unnormalize(){

}

void Trajectory::unnormalize(Point norma){
    for (vector<float>::size_type i = 0; i < x.size(); i++)
		points.push_back(Point((int)(norma.x + norma.x* x[i]), (int)(FRAME_HEIGHT-(y[i] * (FRAME_HEIGHT-norma.y)))));	

    current_point = 0;
}

void Trajectory::normalize(){

}

void Trajectory::normalize(Point norma){
    
}

void Trajectory::saveMovement(string file){
    save_points.open(file);
}

void Trajectory::endSaving(){
    save_points.close();
}

void Trajectory::savePoint(Point pos){
    save_points << index_s << "," << pos.x << "," << pos.y << "\n";
	index_s++;
    save_points.flush();
}

void Trajectory::setNextPoint0(Point pos){
	if((pos.x-points[current_point].x)*(pos.x-points[current_point].x)/(ELIPSE_X) + (pos.y-points[current_point].y)*(pos.y-points[current_point].y)/(ELIPSE_Y) <= 1
			|| (pos.x-points[current_point].x)*(pos.x-points[current_point].x) + (pos.y-points[current_point].y)*(pos.y-points[current_point].y) >  
							(pos.x-points[current_point+1].x)*(pos.x-points[current_point+1].x) + (pos.y-points[current_point+1].y)*(pos.y-points[current_point+1].y)){
		current_point++;

		if (points.size() == current_point) {
			current_point = 0;
		}
	}
}

void Trajectory::setNextPoint1(Point pos){

}

float ruleof3(float x, float x1, float x2, float y1, float y2){
	if (x < x1) 
		return y1;
	if (x > x2)
		return y2;

	return ((x - x1) * (y2 - y1) / (x2 - x1) + y1);
}

char* Trajectory::getError(Point pos){
    float pwm[4], new_send_x, new_send_y;

    if (points[current_point].x - pos.x > 0) {
		pwm[3] = 0;
		pwm[2] = abs(points[current_point].x - pos.x);
	} else {
		pwm[2] = 0;
		pwm[3] = abs(points[current_point].x - pos.x);
	}

	if (points[current_point].y - pos.y > 0) {
		pwm[0] = 0;
		pwm[1] = points[current_point].y - pos.y;
	} else {
		pwm[1] = 0;
		pwm[0] = pos.y - points[current_point].y;
	}

	if (pwm[3] > 10)
		pwm[3] += 50;

	if (pwm[2] > 10)
		pwm[2] += 50;

	pwm[0] = ruleof3(pwm[0],0,240,0,1);//cima
	pwm[1] = ruleof3(pwm[1],0,240,0,-1);//baixo
	pwm[2] = ruleof3(pwm[2],0,240,0,-1);//esquerda
	pwm[3] = ruleof3(pwm[3],0,240,0,1);//direita

	new_send_y = (pwm[0]+pwm[1])*1;
	new_send_x = (pwm[2]+pwm[3])*-1;

	cout << new_send_x << " " << new_send_y << endl;

	sprintf(temp0,"%f",new_send_x);
	sprintf(to_send,"%s",temp0);
	strcat(to_send, ",");
	sprintf(temp1,"%f",new_send_y);
	strcat(to_send, temp1);
	
    cout << to_send << endl;

    return to_send;
}