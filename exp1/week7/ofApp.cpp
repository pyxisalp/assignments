#include "ofApp.h"
#include "waterObj.h"

#define MAX_PATH_SIZE 100
waterObj path(MAX_PATH_SIZE);
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(15); // Limit the speed of our program to 15 frames per second
    
    // We still want to draw on a black background, so we need to draw
    // the background before we do anything with the brush
    ofBackground(255,255,255);
    ofSetLineWidth(4);
    
    draw_flag = 0;
    load_flag = 0;
    flow_flag = 0; // 처음에는 물이 흐르지 않는다.
    dot_diameter = 20.0f;
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(127,23,31);  // Set the drawing color to brown
    
    // Draw shapes for ceiling and floor
    ofDrawRectangle(0, 0, 1024, 40); // Top left corner at (50, 50), 100 wide x 100 high
    ofDrawRectangle(0, 728, 1024, 40); // Top left corner at (50, 50), 100 wide x 100 high
    ofSetLineWidth(5);
    
    ofSetLineWidth(5);
    if( draw_flag ){
        // 선분 그리기
        for (int i = 0; i < num_of_line; i++)
            ofDrawLine(lines[i][0], lines[i][1], lines[i][2], lines[i][3]);
        
        // 점 그리기
        for (int i = 0; i < num_of_dot; i++) {
            ofSetColor(0); // 검정색
            if (i == cur_dot) ofSetColor(255, 0, 0); // 빨간색
                
            ofDrawCircle(dots[i][0], dots[i][1], dot_diameter / 2.0f); // 반지름: 10.0f
        }

        /* COMSIL1-TODO 3 : Draw the line segment and dot in which water starts to flow in the screen.
         Note that after drawing line segment and dot, you have to make selected water start dot colored in red.
         */

        // 2nd week portion.
        ofSetLineWidth(2);
        ofSetColor(0, 0, 255); // 파란색
        if (flow_flag) {
            for (int i = 0; i < path.len; i++)
                // 경로 그리기
                ofDrawLine(path.x[i], path.y[i], path.x[i + 1], path.y[i + 1]);
        }

    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'v') {
        // HACK: only needed on windows, when using ofSetAutoBackground(false)
        glReadBuffer(GL_FRONT);
        ofSaveScreen("savedScreenshot_"+ofGetTimestampString()+".png");
    }
    if (key == 'q'){
        // Reset flags
        draw_flag = 0;
        
        // Free the dynamically allocated memory exits.
        // 선분 동적 할당 해제
        for (int i = 0; i < num_of_line; i++)
            free(lines[i]);
        free(lines);
        cout << "Memory for line segment has been freed." << endl;

        // 점 동적 할당 해제
        for (int i = 0; i < num_of_dot; i++)
            free(dots[i]);
        free(dots);
        cout << "Memory for dot has been freed." << endl;
        
        _Exit(0);
    }
    if (key == 'd'){
        if( !load_flag) return;

        draw_flag = 1; // draw flag 설정
        
        /* COMSIL1-TODO 2: This is draw control part.
        You should draw only after when the key 'd' has been pressed.
        */
    }
    if (key == 's'){
        flow_flag = 1; // 물이 흐른다.
        // 선택한 점에 대해 물이 흐르는 경로를 찾는다.
        path.get_path(dots[cur_dot], lines, num_of_line, ofGetHeight());

        // 2nd week portion.
    }
    if (key == 'e'){
        flow_flag = 0; // 물이 흐르지 않는다.
        // 2nd week portion.
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if( key == 'l'){
        // Open the Open File Dialog
        ofFileDialogResult openFileResult= ofSystemLoadDialog("Select a only txt for Waterfall");
        
        // Check whether the user opened a file
        if( openFileResult.bSuccess){
            ofLogVerbose("User selected a file");
            
            // We have a file, so let's check it and process it
            processOpenFileSelection(openFileResult);
            load_flag = 1;
        }
    }
    
    /* COMSIL1-TODO 4: This is selection dot control part.
     You can select dot in which water starts to flow by left, right direction key (<- , ->).
     */
    
    if (!flow_flag) { //
        if (key == OF_KEY_RIGHT) {
            cur_dot++; // 오른쪽으로 점 이동
            // 오른쪽으로 이동할 수 없으면 맨 왼쪽으로 이동
            if (cur_dot == num_of_dot) cur_dot = 0;

            cout << "Selcted Dot Coordinate is (" << dots[cur_dot][0] << ", " << dots[cur_dot][1] << ")" << endl; // 선택한 점의 좌표 출력
        }
        if (key == OF_KEY_LEFT) {
            cur_dot--; // 왼쪽으로 점 이동
            // 왼쪽으로 이동할 수 없으면 맨 오른쪽으로 이동
            if (cur_dot < 0) cur_dot = num_of_dot - 1;

            cout << "Selcted Dot Coordinate is (" << dots[cur_dot][0] << ", " << dots[cur_dot][1] << ")" << endl; // 선택한 점의 좌표 출력
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
 
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::processOpenFileSelection(ofFileDialogResult openFileResult) {
    //Path to the comma delimited file
    //string fileName = "input.txt";

    string fileName = openFileResult.getName();
    ofFile file(fileName);

    if (!file.exists()) cout << "Target file does not exists." << endl;
    else cout << "We found the target file." << endl;

    ofBuffer buffer(file);

    /* This variable is for indicating which type of input is being received.
     IF input_type == 0, then work of getting line input is in progress.
     IF input_type == 1, then work of getting dot input is in progress.
     */
    int input_type = 0;


    /* COMSIL1-TODO 1 : Below code is for getting the number of line and dot, getting coordinates.
     You must maintain those information. But, currently below code is not complete.
     Also, note that all of coordinate should not be out of screen size.
     However, all of coordinate do not always turn out to be the case.
     So, You have to develop some error handling code that can detect whether coordinate is out of screen size.
    */

    int idx = 0;

    // Read file line by line
    for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
        string line = *it;

        // Split line into strings
        vector<string> words = ofSplitString(line, " ");

        if (words.size() == 1) {
            if (input_type == 0) { // Input for the number of lines.
                num_of_line = atoi(words[0].c_str());
                cout << "The number of line is: " << num_of_line << endl;

                // 선분 동적 할당, 인덱스 0, 1, 2, 3 = x1, y1, x2, y2
                lines = (int**)malloc(sizeof(int*) * num_of_line);
                for (int i = 0; i < num_of_line; i++)
                    lines[i] = (int*)malloc(sizeof(int) * 4);
            }
            else { // Input for the number of dots.
                num_of_dot = atoi(words[0].c_str());
                cout << "The number of dot is: " << num_of_dot << endl;

                // 점 동적 할당, 인덱스 0, 1 = x1, y1
                dots = (int**)malloc(sizeof(int*) * num_of_dot);
                for (int i = 0; i < num_of_dot; i++)
                    dots[i] = (int*)malloc(sizeof(int) * 2);
            }
        }
        else if (words.size() >= 2) {
            int x1, y1, x2, y2;
            if (input_type == 0) { // Input for actual information of lines
                x1 = atoi(words[0].c_str());
                y1 = atoi(words[1].c_str());
                x2 = atoi(words[2].c_str());
                y2 = atoi(words[3].c_str());

                // 좌표가 윈도우를 벗어나면 에러 메시지를 출력한다.
                if (x1 < 0 || x1 > 1024) {
                    cout << "x1 is out of screen: " << idx + 1 << " line" << endl;
                    return;
                }
                if (y1 < 0 || y1 > 1024) {
                    cout << "y1 is out of screen: " << idx + 1 << " line" << endl;
                    return;
                }
                if (x2 < 0 || x2 > 1024) {
                    cout << "x2 is out of screen: " << idx + 1 << " line" << endl;
                    return;
                }
                if (y2 < 0 || y2 > 1024) {
                    cout << "y2 is out of screen: " << idx + 1 << " line" << endl;
                    return;
                }

                lines[idx][0] = x1;
                lines[idx][1] = y1;
                lines[idx][2] = x2;
                lines[idx][3] = y2;

                idx++;
                if (idx == num_of_line) {
                    input_type = 1;
                    idx = 0;
                }
            }
            else { // Input for actual information of dots.
                x1 = atoi(words[0].c_str());
                y1 = atoi(words[1].c_str());

                // 좌표가 윈도우를 벗어나면 에러 메시지를 출력한다.
                if (x1 < 0 || x1 > 1024) {
                    cout << "x1 is out of screen: " << idx + 1 << " dot" << endl;
                    return;
                }
                if (y1 < 0 || y1 > 1024) {
                    cout << "y1 is out of screen: " << idx + 1 << " dot" << endl;
                    return;
                }

                dots[idx][0] = x1;
                dots[idx][1] = y1;

                idx++;
                if (idx == num_of_dot)
                    input_type = 0;
            }
        } // End of else if.
    } // End of for-loop (Read file line by line).


    // 점 입력이 오름차순 정렬이 아닐 수 있으므로, 선택 정렬(selection sort)을 통해 좌표를 정렬한다.
    int temp_x, temp_y, min_idx;

    // x 좌표 오름차순 정렬
    for (int i = 0; i < num_of_dot - 1; i++) {
        min_idx = i;
        
        for (int j = i + 1; j < num_of_dot; j++) {
            if (dots[i][0] < dots[min_idx][0])
                min_idx = j;
        }

        temp_x = dots[min_idx][0];
        dots[min_idx][0] = dots[i][0];
        dots[i][0] = temp_x;

        temp_y = dots[min_idx][1];
        dots[min_idx][1] = dots[i][1];
        dots[i][1] = temp_y;
    }

    // y 좌표 오름차순 정렬
    for (int i = 0; i < num_of_dot - 1; i++) {
        min_idx = i;

        for (int j = i + 1; j < num_of_dot; j++) {
            if (dots[i][1] < dots[min_idx][1])
                min_idx = j;
        }

        temp_x = dots[min_idx][0];
        dots[min_idx][0] = dots[i][0];
        dots[i][0] = temp_x;

        temp_y = dots[min_idx][1];
        dots[min_idx][1] = dots[i][1];
        dots[i][1] = temp_y;
    }
        
    //initializeWaterLines();
}

void ofApp::initializeWaterLines() {
    ;
}


