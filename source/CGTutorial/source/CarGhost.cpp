#include "CarGhost.hpp"

CarGhost::CarGhost(const std::string& file, Model carModel): carModel(carModel)
{
    glm::mat4 initialCarModelMatrix(0.0f);
    carModelMatrix = &initialCarModelMatrix;
    
    HistoryEntry initialHistoryEntry;
    initialHistoryEntry.time = 0.0;
    initialHistoryEntry.carMatrix = initialCarModelMatrix;
    history.push_back(initialHistoryEntry);
    
    readRecordFile(file);
    currentPosition = 0;
    currentTimestamp = previousTimestamp = history[0].time;
}

glm::mat4 CarGhost::getModelMatrix(){
    return *carModelMatrix;
}

void CarGhost::readRecordFile(const std::string& file){
    HistoryEntry currHistoryEntry;
    string line;
    string skip;
    ifstream fIn (file);
    if (fIn.is_open())
    {
        // read timestamps
        getline (fIn,line); // ignore "timestamps {\n"
        int lap = 0;
        while(lap < LAPS && (getline (fIn,line) && line != "}\n"))
        {
            std::istringstream in(line);
            in.ignore(4, EOF); // ignore "Lap "
            in >> lap;
            in.ignore(2, EOF); // ignore ": "
            float currTimestamp;
            in >> currTimestamp;
            recordTimestamps.push_back(currTimestamp);
//            cout << "recordTimestamps[" << lap-1 << "]: " << recordTimestamps[lap-1] << "\n";
            in.ignore(2, '\n'); // ignore ","
        }
        getline (fIn,line); // ignore "}\n"
        getline (fIn,line); // ignore "history {\n"
        
        while ( getline (fIn,line) ) //read stream line by line
        {
//            cout << line << '\n';
            // read historyEntry
            if(line.size() > 0){
                std::istringstream in(line);      //make a stream for the line itself
                
                in >> currHistoryEntry.time; // read time
                
                in.ignore(4, EOF); // ignore " : ("
                
                // read carMatrix
                for(int i = 0; i <= 3; i++){
                    in.ignore(1, EOF); // ignore "("
                    for(int j = 0; j <= 2; j++){
                        in >> currHistoryEntry.carMatrix[i][j];
                        in.ignore(2, EOF); // ignore ", "
                    }
//                    in >> currHistoryEntry.carMatrix[i][0];
//                    in.ignore(2, EOF); // ignore ", "
//                    in >> currHistoryEntry.carMatrix[i][1];
//                    in.ignore(2, EOF); // ignore ", "
//                    in >> currHistoryEntry.carMatrix[i][2];
//                    in.ignore(2, EOF); // ignore ", "
                    
                    in >> currHistoryEntry.carMatrix[i][3];
                    in.ignore(3, '\n'); // ignore "), " or newline
                }
            }
            this->history.push_back(currHistoryEntry);
        }
        fIn.close();
    }
    else cout << "readRecordFile: Unable to open file\n";
}

std::vector<float> CarGhost::getRecordTimestamps(){
    return recordTimestamps;
}


void CarGhost::updateCarMatrixWithHistory(double time){
    while(time > currentTimestamp && currentPosition < history.size()){
        previousTimestamp = currentTimestamp;
        currentPosition++;
        currentTimestamp = history[currentPosition].time;
    }    
    carModelMatrix = &history[currentPosition > 0? currentPosition - 1 : currentPosition].carMatrix;
}

void CarGhost::draw(Shader shader, double time){
    updateCarMatrixWithHistory(time);
    carModel.Draw(shader);
    
//    cout << "currentPosition: " + std::to_string(currentPosition) + "\n";
//    cout << "currentTimestamp: " + std::to_string(currentTimestamp) + "\n";
//    cout << "Timstamp from history: " + std::to_string(history[currentPosition].time) + "\n";
//    cout << "carModelMatrix: " + glm::to_string(*carModelMatrix) + "\n";
}
