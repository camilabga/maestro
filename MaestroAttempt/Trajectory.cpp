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
    ifstream myfile;
    myfile.open(file,std::ifstream::in);

    char temp0[10], temp1[10], temp2[10];
    //char temp1[10], temp2[10];
    x.clear();
    y.clear();
    min.clear();
    delta.clear();
    velx.clear();
    vely.clear();

    if (myfile.is_open()){
        while (myfile.good()) {
            myfile.getline (temp0, 10, ',' );
            myfile.getline (temp1, 10, ',' );
            myfile.getline (temp2, 10);
            x.push_back(atof(temp1));
            y.push_back(atof(temp2));
            std::cerr<<temp1 <<"! " << temp2 <<" ";

        }

        std::cerr<<x.size() <<"! " << y.size() <<"\n";

        myfile.close();
    }
}

void Trajectory::getPointsFromCSV2(string file){
    ifstream myfile;
    myfile.open(file,std::ifstream::in);

    //char temp0[10], temp1[10], temp2[10];
    int temp;
    float temp1,temp2;
    //char temp1[10], temp2[10];
    x.clear();
    y.clear();
    min.clear();
    delta.clear();
    velx.clear();
    vely.clear();

    myfile.ignore(numeric_limits<streamsize>::max(), '[');
    myfile>>temp;
    std::cout<<temp<<'\n';
    min.push_back(temp);
    myfile.ignore(numeric_limits<streamsize>::max(), ';');
    myfile>>temp;
    std::cout<<temp<<'\n';
    min.push_back(temp);
    myfile.ignore(numeric_limits<streamsize>::max(), ';');
    myfile>>temp;
    std::cout<<temp<<'\n';
    min.push_back(temp);
    myfile.ignore(numeric_limits<streamsize>::max(), ';');
    myfile>>temp;
    std::cout<<temp<<'\n';
    min.push_back(temp);
    myfile.ignore(numeric_limits<streamsize>::max(), '[');

    myfile>>temp;
    std::cout<<temp<<'\n';
    delta.push_back(temp);
    myfile.ignore(numeric_limits<streamsize>::max(), ';');
    myfile>>temp;
    std::cout<<temp<<'\n';
    delta.push_back(temp);
    myfile.ignore(numeric_limits<streamsize>::max(), ';');
    myfile>>temp;
    std::cout<<temp<<'\n';
    delta.push_back(temp);
    myfile.ignore(numeric_limits<streamsize>::max(), ';');
    myfile>>temp;
    std::cout<<temp<<'\n';
    delta.push_back(temp);
    myfile.ignore(numeric_limits<streamsize>::max(), '\n');
    myfile.ignore(numeric_limits<streamsize>::max(), '\n');

    if (myfile.is_open()){
        while (myfile.good()) {
            myfile >> temp1;
            x.push_back(temp1);
            myfile.ignore(numeric_limits<streamsize>::max(), ',');
            myfile >> temp2;
            y.push_back(temp2);
            myfile.ignore(numeric_limits<streamsize>::max(), ',');
            myfile >> temp1;
            velx.push_back(temp1);
            myfile.ignore(numeric_limits<streamsize>::max(), ',');
            myfile >> temp2;
            vely.push_back(temp2);

            std::cerr<<temp1 <<" | " << temp2 <<'\n';
            myfile.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        std::cerr<<x.size()<<"\n";

        myfile.close();
    }
}

void Trajectory::unnormalize(){

}

void Trajectory::unnormalize(Point norma){
    for (vector<float>::size_type i = 0; i < x.size(); i++){
        points.push_back(Point((int)(norma.x + norma.x* x[i]), (int)(FRAME_HEIGHT-(y[i] * (FRAME_HEIGHT-norma.y)))));
        std::cout<<"Points: "<< points[i]<<'\n';
    }

    current_point = 0;
}

void Trajectory::unnormalize2(){
    for (vector<float>::size_type i = 0; i < x.size(); i++){
        std::cout<<"Point "<<i <<" = "<< x[i]<<" , "<<y[i]<<'\n';
        points.push_back(Point((int)(min[0] + delta[0]* x[i]), (int)(min[1] + delta[1]* y[i])));
        vel.push_back(Point((int)(min[2] + delta[2]* velx[i]), (int)(min[3] + delta[3]* vely[i])));
        std::cout<<"Point "<<i <<" = "<< points[i]<<'\n';
    }

    current_point = 0;
}

void Trajectory::normalize(){

}

void Trajectory::normalize(Point norma){
    int width = norma.x;
    int height = norma.y;

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

void Trajectory::setNextPoint0(Point pos, int sens){
    // Change OR
    unsigned int oldId = current_point;
    unsigned int newId = (points.size()-1 == oldId ? 0 : oldId+1);
    double pointElipseX = (pos.x-points[oldId].x)*(pos.x-points[oldId].x)/(ELIPSE_X);
    double pointElipseY = (pos.y-points[newId].y)*(pos.y-points[newId].y)/(ELIPSE_Y);
    double thisPointDist = (pos.x-points[oldId].x)*(pos.x-points[oldId].x) + (pos.y-points[oldId].y)*(pos.y-points[oldId].y);
    double nextPointDist = (pos.x-points[newId].x)*(pos.x-points[newId].x) + (pos.y-points[newId].y)*(pos.y-points[newId].y);
    if(pointElipseX + pointElipseY <= sens && thisPointDist > nextPointDist){
        current_point = newId;
    }
//    cerr << "1st: " << (pointElipseX + pointElipseY) <<
//            "  2nd: " << (thisPointDist > nextPointDist)
//         << endl;
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

    string textX, textY;
    textX = (new_send_x < 0 ? "Esquerda" : "Direita");
    textY = (new_send_y < 0 ? "Cima" : "Baixo");
    cout << textX << " " << textY << endl;
    //cout << new_send_x << " " << new_send_y << endl;

    setlocale(LC_NUMERIC, "en_US.UTF-8");
    sprintf(temp0,"%f",new_send_x);

    //cout << temp0<<endl;

    sprintf(to_send,"%s",temp0);
    strcat(to_send, ";");
    sprintf(temp1,"%.3f",new_send_y);
    strcat(to_send, temp1);

    //cout << to_send << endl;

    return to_send;
}
