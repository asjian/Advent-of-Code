#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <set>
#include <iterator>
using namespace std;

class Image {
    private:
    int ID;
    int angle = 0;
    bool flipped = false;
    vector<vector<char>> image;
    vector<pair<int,tuple<int,int,bool>>> adjacencies;

    void rotate_image(int theta) {
        while(theta<0) {
        theta += 360;
        }
        theta = theta%360;
        vector<vector<char>> rotatedimage;
        vector<char> currline;

        if(theta == 0) {/*do nothing*/}

        else if(theta == 90) { //90 CCW
            for(int j = image.at(0).size();j>0;j--) {
                for(int i=0;i<image.size();i++) {
                    currline.push_back(image.at(i).at(j-1));
                }
                rotatedimage.push_back(currline);
                currline.clear();
            }
            image = rotatedimage;
        }
        else if(theta == 180) {
            if(image.size()%2 == 0) {
                for(int i=0; i<image.size()/2;i++) {
                    for(int j=0;j<image.at(i).size();j++) {
                        char temp = image.at(i).at(j);
                        image.at(i).at(j) = image.at(image.size()-1-i).at(image.at(i).size()-1-j);
                        image.at(image.size()-1-i).at(image.at(i).size()-1-j) = temp;
                    }
                }
            }
            else {
                for(int i=0; i<image.size()/2;i++) {
                    for(int j=0;j<image.at(i).size();j++) {
                        char temp = image.at(i).at(j);
                        image.at(i).at(j) = image.at(image.size()-1-i).at(image.at(i).size()-1-j);
                        image.at(image.size()-1-i).at(image.at(i).size()-1-j) = temp;
                    }
                }
                int x = image.size()/2 + 1;
                for(int y=0; y<image.at(x).size()/2;y++) {
                    char temp = image.at(x).at(y);
                    image.at(x).at(y) = image.at(image.size()-1-x).at(image.at(x).size()-1-y);
                    image.at(image.size()-1-x).at(image.at(x).size()-1-y) = temp;
                }
            }
        }
        else if(theta == 270) {
            for(int j=0;j<image.at(0).size();j++) {
                for(int i=image.size();i>0;i--) {
                    currline.push_back(image.at(i-1).at(j));
                }
                rotatedimage.push_back(currline);
                currline.clear();
            }
            image = rotatedimage;
        }
        else {
            cout<<"Invalid Rotation Angle - rotate_image()!"<< endl;
        }
    }
    void flip_image() {
        for(int i=0;i<image.size();i++) {
            for(int j=0;j<image.at(i).size()/2;j++) {
                char temp = image.at(i).at(j);
                image.at(i).at(j) = image.at(i).at(image.at(i).size()-1-j);
                image.at(i).at(image.at(i).size()-1-j) = temp;
            }
        }
    }
    public:

    Image(int ID_in,vector<vector<char>> &img_in): ID(ID_in),image(img_in) {}

    int getID() {
        return ID;
    }
    int getangle() {
        return angle;
    }
    bool getflipped() {
        return flipped;
    }
    void print_image() {
        for(int i=0; i<image.size();i++) {
            for(int j=0;j<image.at(i).size();j++) {
                cout<<image.at(i).at(j);
            }
            cout<<endl;
        }
    }
    vector<vector<char>> & getImage() {
        return image;
    }
    vector<pair<int,tuple<int,int,bool>>> & getadjacencies() {
        return adjacencies;
    }
    void add_adjacency(pair<int,tuple<int,int,bool>> &border) {
        adjacencies.push_back(border);
    }
    void rotate(int theta) { //rotates counterclockwise by theta degrees
        angle = (angle + theta)%360;
        for(int i=0; i<adjacencies.size();i++) {
            get<0>(adjacencies.at(i).second) = (get<0>(adjacencies.at(i).second) + theta)%360; //update which edge
            get<1>(adjacencies.at(i).second) = (get<1>(adjacencies.at(i).second) + theta)%360; //update edge's orientation
        }
        rotate_image(theta);
    }
    void flip() {
        flipped = !flipped;
         for(int i=0; i<adjacencies.size();i++) {
            if(get<0>(adjacencies.at(i).second) == 0 || get<0>(adjacencies.at(i).second) == 180) {
                get<0>(adjacencies.at(i).second) = (get<0>(adjacencies.at(i).second) + 180)%360; //update which edge 
                get<2>(adjacencies.at(i).second) = !get<2>(adjacencies.at(i).second); //update edge's flippedness
                get<1>(adjacencies.at(i).second) = (360-get<1>(adjacencies.at(i).second))%360; //update edge's orientation
             }
            else if (get<0>(adjacencies.at(i).second) == 90 || get<0>(adjacencies.at(i).second) == 270) {
                get<2>(adjacencies.at(i).second) = !get<2>(adjacencies.at(i).second); //update edge's flippedness
                get<1>(adjacencies.at(i).second) = (360-get<1>(adjacencies.at(i).second))%360; //update edge's orientation
             }
            else {
                 cout<<"Edge orientation is invalid"<<endl;
             }
         }
        flip_image();
    }
    pair<int,tuple<int,int,bool>> get_border(int angle) { //angle tells which side
        for(int i=0; i<adjacencies.size();i++) {
            if(get<0>(adjacencies.at(i).second) == angle) {
                return adjacencies.at(i);
            }
        }
        return {-999,{-999,-999,false}};
    }
};
const char MONSTER_SYMBOL = '#';
const char MONSTER_MARK = 'O';
int IMAGE_HEIGHT; //Height and Width of fully constructed image
int IMAGE_WIDTH;  //1 unit of Height/Width is 1 Tile
int NUM_IMAGES;
unordered_map<int,Image*> imagedict;
vector<pair<int,Image*>> orderedimages; //I'm using pointers so I only need to free memory once
vector<vector<char>> trimmedgrid;

bool check_borders(const vector<char> &c1, const vector<char> &c2,bool reverse = false) {
    assert(c1.size() == c2.size());
    char compchar;
    for(int i=0; i<c1.size();i++) {
        compchar = c2.at(i);
        if(reverse) {
            compchar = c2.at(c2.size()-1-i);
        }
        if (c1.at(i)!=compchar) {
            return false;
        }
    }
    return true;
}
vector<char> get_row(const vector<vector<char>> &img, const string &side) {
    if(side == "top") {
        return img.at(0);
    }
    else if(side == "bottom") {
        return img.at(9);
    }
    cout<<"bullshit"<<endl;
    return {};
}
vector<char> get_column(const vector<vector<char>> &img, const string &side) {
    vector<char> column;

    for(int i=0; i<img.size();i++) {
        if(side == "left") {
            column.push_back(img.at(i).at(0));
        }
        else if(side == "right") {
            column.push_back(img.at(i).at(9));
        }
    }
    return column;
}
void print_row(vector<char> &row) {
    for(int i=0; i<row.size();i++) {
        cout<<row.at(i);
    }
}
void print_grid_with_borders(int TOTAL_HEIGHT = IMAGE_HEIGHT, int TOTAL_WIDTH = IMAGE_WIDTH) {
    int block_height = orderedimages.at(0).second->getImage().size();

    for(int shift=0;shift<orderedimages.size();shift+=TOTAL_HEIGHT) {
        for(int a=0;a<TOTAL_WIDTH;a++) {
            if(a+shift<orderedimages.size()) {
            cout<<orderedimages.at(a+shift).first << "       ";
            }
        }
        cout<<endl;
        for(int j=0; j<block_height; j++) {
            for(int i=0; i<TOTAL_WIDTH;i++) {
                if(i+shift<orderedimages.size()) {
                print_row(orderedimages.at(shift+i).second->getImage().at(j));
                cout<<" ";
                }
            }
            cout<<endl;
        }
        cout<<endl;
    }
}
void rotate_grid(int theta) {
    while(theta<0) {
    theta += 360;
    }
    theta = theta%360;
    vector<vector<char>> rotatedgrid;
    vector<char> currline;

    if(theta == 0) {/*do nothing*/}

    else if(theta == 90) { //90 CCW
        for(int j = trimmedgrid.at(0).size();j>0;j--) {
            for(int i=0;i<trimmedgrid.size();i++) {
                currline.push_back(trimmedgrid.at(i).at(j-1));
            }
            rotatedgrid.push_back(currline);
            currline.clear();
        }
        trimmedgrid = rotatedgrid;
    }
    else if(theta == 180) {
        if(trimmedgrid.size()%2 == 0) {
            for(int i=0; i<trimmedgrid.size()/2;i++) {
                for(int j=0;j<trimmedgrid.at(i).size();j++) {
                    char temp = trimmedgrid.at(i).at(j);
                    trimmedgrid.at(i).at(j) = trimmedgrid.at(trimmedgrid.size()-1-i).at(trimmedgrid.at(i).size()-1-j);
                    trimmedgrid.at(trimmedgrid.size()-1-i).at(trimmedgrid.at(i).size()-1-j) = temp;
                }
            }
        }
        else {
            for(int i=0; i<trimmedgrid.size()/2;i++) {
                for(int j=0;j<trimmedgrid.at(i).size();j++) {
                    char temp = trimmedgrid.at(i).at(j);
                    trimmedgrid.at(i).at(j) = trimmedgrid.at(trimmedgrid.size()-1-i).at(trimmedgrid.at(i).size()-1-j);
                    trimmedgrid.at(trimmedgrid.size()-1-i).at(trimmedgrid.at(i).size()-1-j) = temp;
                }
            }
            int x = trimmedgrid.size()/2 + 1;
            for(int y=0; y<trimmedgrid.at(x).size()/2;y++) {
                char temp = trimmedgrid.at(x).at(y);
                trimmedgrid.at(x).at(y) = trimmedgrid.at(trimmedgrid.size()-1-x).at(trimmedgrid.at(x).size()-1-y);
                trimmedgrid.at(trimmedgrid.size()-1-x).at(trimmedgrid.at(x).size()-1-y) = temp;
            }
        }
    }
    else if(theta == 270) {
        for(int j=0;j<trimmedgrid.at(0).size();j++) {
            for(int i=trimmedgrid.size();i>0;i--) {
                currline.push_back(trimmedgrid.at(i-1).at(j));
            }
            rotatedgrid.push_back(currline);
            currline.clear();
        }
        trimmedgrid = rotatedgrid;
    }
    else {
        cout<<"Invalid Rotation Angle - rotategrid()!"<< endl;
    }
}
void flip_grid() {
    for(int i=0;i<trimmedgrid.size();i++) {
        for(int j=0;j<trimmedgrid.at(i).size()/2;j++) {
            char temp = trimmedgrid.at(i).at(j);
            trimmedgrid.at(i).at(j) = trimmedgrid.at(i).at(trimmedgrid.at(i).size()-1-j);
            trimmedgrid.at(i).at(trimmedgrid.at(i).size()-1-j) = temp;
        }
    }
}
void print_trimmed_grid() {
    for(int i=0;i<trimmedgrid.size();i++) {
        for(int j=0;j<trimmedgrid.at(i).size();j++) {
            cout<<trimmedgrid.at(i).at(j);
        }
        cout<<endl;
    }
}
void append_trimmed_row(vector<char> &currline, vector<char> &row) {
    for(int i=1;i<row.size()-1;i++) {
        currline.push_back(row.at(i)); //adds to the end of currline,even if stuff is already in there
    }
}
void build_trimmed_grid() {
   int block_height = orderedimages.at(0).second->getImage().size();
   vector<char> currline; 
   
   for(int shift=0;shift<orderedimages.size();shift+=IMAGE_HEIGHT) {
        for(int j=1; j<block_height-1; j++) {
            for(int i=0; i<IMAGE_WIDTH;i++) {
                if(i+shift<orderedimages.size()) {
                append_trimmed_row(currline,orderedimages.at(shift+i).second->getImage().at(j));
                }
            }
            trimmedgrid.push_back(currline);
            currline.clear();
        }
    }
}
tuple<int,int,bool> check_adjacent(const pair<int,Image*> &img1pair, const pair<int,Image*> &img2pair) {
    vector<vector<char>> &img = img1pair.second->getImage();
    vector<vector<char>> &img2 = img2pair.second->getImage();

    vector<char> toprow1 = get_row(img,"top");
    vector<char> botrow1 = get_row(img,"bottom");
    vector<char> leftcol1 = get_column(img,"left");
    vector<char> rightcol1 = get_column(img,"right");

    vector<char> toprow2 = get_row(img2,"top");
    vector<char> botrow2 = get_row(img2,"bottom");
    vector<char> leftcol2 = get_column(img2,"left");
    vector<char> rightcol2 = get_column(img2,"right");

    tuple<int,int,bool> returntuple = {-999,-999,false};

    if(check_borders(rightcol1,rightcol2,false)) {
        returntuple = {0,0,true};
    }
    else if(check_borders(rightcol1,rightcol2,true)) {
        returntuple = {0,180,false};
    }
    else if(check_borders(rightcol1,toprow2,false)) {
        returntuple = {0,90,true};
    }
    else if(check_borders(rightcol1,toprow2,true)) {
        returntuple = {0,90,false};
    }
    else if(check_borders(rightcol1,leftcol2,false)) {
        returntuple = {0,0,false};
    }
    else if(check_borders(rightcol1,leftcol2,true)) {
        returntuple = {0,180,true};
    }
    else if(check_borders(rightcol1,botrow2,false)) {
        returntuple = {0,270,false};
    }
    else if(check_borders(rightcol1,botrow2,true)) {
        returntuple = {0,270,true};
    }

    else if(check_borders(toprow1,rightcol2,false)) {
        returntuple = {90,90,true};
    }
    else if(check_borders(toprow1,rightcol2,true)) {
        returntuple = {90,270,false};
    }
    else if(check_borders(toprow1,toprow2,false)) {
        returntuple = {90,180,true};
    }
    else if(check_borders(toprow1,toprow2,true)) {
        returntuple = {90,180,false};
    }
    else if(check_borders(toprow1,leftcol2,false)) {
        returntuple = {90,90,false};
    }
    else if(check_borders(toprow1,leftcol2,true)) {
        returntuple = {90,270,true};
    }
    else if(check_borders(toprow1,botrow2,false)) {
        returntuple = {90,0,false};
    }
    else if(check_borders(toprow1,botrow2,true)) {
        returntuple = {90,0,true};
    }

    else if(check_borders(leftcol1,rightcol2,false)) {
        returntuple = {180,0,false};
    }
    else if(check_borders(leftcol1,rightcol2,true)) {
        returntuple = {180,180,true};
    }
    else if(check_borders(leftcol1,toprow2,false)) {
        returntuple = {180,270,false};
    }
    else if(check_borders(leftcol1,toprow2,true)) {
        returntuple = {180,270,true};
    }
    else if(check_borders(leftcol1,leftcol2,false)) {
        returntuple = {180,0,true};
    }
    else if(check_borders(leftcol1,leftcol2,true)) {
        returntuple = {180,180,false};
    }
    else if(check_borders(leftcol1,botrow2,false)) {
        returntuple = {180,90,true};
    }
    else if(check_borders(leftcol1,botrow2,true)) {
        returntuple = {180,90,false};
    }

    else if(check_borders(botrow1,rightcol2,false)) {
        returntuple = {270,90,false};
    }
    else if(check_borders(botrow1,rightcol2,true)) {
        returntuple = {270,270,true};
    }
    else if(check_borders(botrow1,toprow2,false)) {
        returntuple = {270,0,false};
    }
    else if(check_borders(botrow1,toprow2,true)) {
        returntuple = {270,0,true};
    }
    else if(check_borders(botrow1,leftcol2,false)) {
        returntuple = {270,90,true};
    }
    else if(check_borders(botrow1,leftcol2,true)) {
        returntuple = {270,270,false};
    }
    else if(check_borders(botrow1,botrow2,false)) {
        returntuple = {270,180,true};
    }
    else if(check_borders(botrow1,botrow2,true)) {
        returntuple = {270,180,false};
    }

    return returntuple;
}
void add_adjacencies() {
    tuple<int,int,bool> borderinfo;
    pair<int,tuple<int,int,bool>> IDandBorder;

    for(auto &pair1: imagedict) {
        for(auto &pair2: imagedict) {
            if(pair1.first != pair2.first) {
                borderinfo = check_adjacent(pair1,pair2);

                if(get<1>(borderinfo) != -999) {
                    IDandBorder = {pair2.first,borderinfo};
                    pair1.second->add_adjacency(IDandBorder);
                }
            }
        }
    }
}
vector<int> get_all_corner_IDs() {
    return {orderedimages.at(0).first,orderedimages.at(IMAGE_WIDTH-1).first, 
        orderedimages.at(IMAGE_HEIGHT*IMAGE_WIDTH-IMAGE_WIDTH).first,orderedimages.at(IMAGE_HEIGHT*IMAGE_WIDTH-1).first};
}
pair<int,Image*> find_best_corner() {
    for(auto &pair: imagedict) {
        if(pair.second->getadjacencies().size()==2) {
            if(get<0>(pair.second->getadjacencies().at(0).second)%360 == 0 or get<0>(pair.second->getadjacencies().at(0).second)%360 == 270) {
                if(get<0>(pair.second->getadjacencies().at(1).second)%360 == 0 || get<0>(pair.second->getadjacencies().at(1).second)%360 == 270) {
                    return pair;
                }
            }
        }
    }
    return {-999,nullptr};
}
pair<int,Image*> find_corner() {
    for(auto &pair: imagedict) {
        if(pair.second->getadjacencies().size()==2) {
            return pair;
        }
    }
    return {-999,nullptr};
}
void reorient_corner(pair<int,Image*> &cornerpair) {
        set<int> pos1 = {0,90};
        set<int> pos2 = {90,180};
        set<int> pos3 = {180,270};

        if(pos1.find(get<0>(cornerpair.second->getadjacencies().at(0).second)) != pos1.end() &&
        pos1.find(get<0>(cornerpair.second->getadjacencies().at(1).second)) != pos1.end()) {
            cornerpair.second->rotate(270);
        }

        else if(pos2.find(get<0>(cornerpair.second->getadjacencies().at(0).second)) != pos2.end() &&
        pos2.find(get<0>(cornerpair.second->getadjacencies().at(1).second)) != pos2.end()) {
            cornerpair.second->rotate(180);
        }

        else if(pos3.find(get<0>(cornerpair.second->getadjacencies().at(0).second)) != pos3.end() &&
        pos3.find(get<0>(cornerpair.second->getadjacencies().at(1).second)) != pos3.end()) {
            cornerpair.second->rotate(90);
        }
        else {
            cout<<"something is wrong with the corner: borders should be adjacent"<<endl;
        } 
}
pair<int,Image*> reorient_image(pair<int,tuple<int,int,bool>> &imageinfo) { //take the adjustments specified by the tuple
    int imageID = imageinfo.first;                                         //and apply them to the image with the given ID
    int rotationangle = get<1>(imageinfo.second);
    bool toflip = get<2>(imageinfo.second);
    Image* imgptr = imagedict[imageID];

    if(toflip) {
        imgptr->flip();
    }
    imgptr->rotate(rotationangle);  
    return {imageID,imgptr};       
}                                           
void construct_grid() {
    pair<int,Image*> topleftpair;
    topleftpair = find_best_corner();
    if(topleftpair.first == -999) {
        topleftpair = find_corner();
        reorient_corner(topleftpair);
    }
    orderedimages.push_back(topleftpair);
    IMAGE_HEIGHT = 0;
    IMAGE_WIDTH = 0;
    //continue by iteratively rotating/flipping the right-border until you hit an edge, then go downward, etc
    pair<int,tuple<int,int,bool>> nextimageinfo = topleftpair.second->get_border(0);
    pair<int,Image*> imgpair;

    for(int i=1;i<NUM_IMAGES;i++) {
        imgpair = reorient_image(nextimageinfo);
        orderedimages.push_back(imgpair);      

        nextimageinfo = imgpair.second->get_border(0); //get right border
        if(nextimageinfo.first == -999) { //right border doesn't exist, it's a right-edge
            if(IMAGE_HEIGHT == 0) {
                IMAGE_WIDTH = i+1;
            }
            if(i!=NUM_IMAGES-1) {
            nextimageinfo = orderedimages.at(i+1-IMAGE_WIDTH).second->get_border(270); //go to the leftmost image in the row and go down
            }
            IMAGE_HEIGHT++;
        }
    }
}
int count_symbols(vector<vector<char>> &grid,char symbol) {
    int count = 0;
    for(int i=0;i<grid.size();i++) {
        for(int j=0;j<grid.at(0).size();j++) {
            if(grid.at(i).at(j) == symbol) {
                count++;
            }
        }
    }
    return count;
}
bool check_coordinate(int x, int y,char monster_symbol) {
    if(x>=0 && x<trimmedgrid.size()) {
        if(y>=0 && y<trimmedgrid.at(x).size()) {
            if (trimmedgrid.at(x).at(y) == monster_symbol) {
                return true;
            }
        }
    }
    return false;
}
pair<int,int> get_string_dimensions(const string &str) {
    int height = 0;
    int width = 0;
    for(char c:str) {
        if(c=='\n'){
            height++;
        }
        else {
            if(height == 0) {
                width++;
            }
        }
    }
    return {height,width};
}
vector<pair<int,int>> get_monster_coordinates(const string &monster,const pair<int,int> &dimensions,const char monster_symbol) {
    vector<pair<int,int>> coordinates;
    for(int i=0;i<monster.length();i++) {
        if(monster.at(i) == monster_symbol) {
            coordinates.push_back({i/dimensions.second,i%dimensions.second});
        }
    }
    return coordinates;
}
bool check_box(pair<int,int> &start_coords, vector<pair<int,int>> &coords_to_check, char monster_symbol, char monster_mark) {
    for(pair<int,int> &coords:coords_to_check) {
        if(!check_coordinate(start_coords.first + coords.first,start_coords.second + coords.second, monster_symbol)) {
            return false;
        } 
    } 
    for(pair<int,int> &coords2:coords_to_check) {
        trimmedgrid.at(start_coords.first+coords2.first).at(start_coords.second+coords2.second) = monster_mark;
    } 
    return true;
}
int find_seamonsters() {
    string SEAMONSTER = "xxxxxxxxxxxxxxxxxx#x\n#xxxx##xxxx##xxxx###\nx#xx#xx#xx#xx#xx#xxx\n";

    pair<int,int> dimensions = get_string_dimensions(SEAMONSTER);
    SEAMONSTER.erase(remove(SEAMONSTER.begin(), SEAMONSTER.end(), '\n'), SEAMONSTER.end());
    vector<pair<int,int>> coordinates = get_monster_coordinates(SEAMONSTER,dimensions,MONSTER_SYMBOL);
    
    int seamonster_count = 0;
    pair<int,int> current_coords;

    for(int flip=0;flip<2;flip++) {
    for(int rot=0;rot<4;rot++) {
        for(int i=0;i<trimmedgrid.size()-dimensions.first;i++) {
            for(int j=0; j<trimmedgrid.size()-dimensions.second;j++) {
                current_coords = {i,j};
                if(check_box(current_coords,coordinates,MONSTER_SYMBOL,MONSTER_MARK)) {
                    seamonster_count++;
                }
            }
        }
        if(seamonster_count>0) 
            break;
        rotate_grid(90);
    }
        if(seamonster_count>0) 
            break;
        flip_grid();
    }
    return seamonster_count;
}
int main() {
    string filename;
    cin >> filename;
    ifstream fin(filename);
    if(!fin.is_open()) {
        std::cout<<"Error opening file"<<endl;
        return -1;
    }
    string imgstr;
    string label = "Tile";
    string digits = "0123456789";

    vector<vector<char>> currimg;
    vector<char> currline;
    int id = 0;

    fin>>imgstr;
    while(fin>>imgstr) {
        if(digits.find(imgstr.at(0)) != -1) {
        id = atoi(imgstr.substr(0,imgstr.length()-1).c_str());
        }
        else if(imgstr != label) {
          for(char c:imgstr) {
                currline.push_back(c);
            }
            currimg.push_back(currline);
            currline.clear();
        }
        else {
        imagedict[id] = new Image(id,currimg);
        currimg.clear();
        }
    }
    imagedict[id] = new Image(id,currimg);
    currimg.clear();
    NUM_IMAGES = imagedict.size();

    add_adjacencies();
    construct_grid();
    vector<int> cornerIDs = get_all_corner_IDs(); //Part 1 Done
    
    build_trimmed_grid();
    int numseamonsters = find_seamonsters();
    int waterroughness = count_symbols(trimmedgrid,MONSTER_SYMBOL); //Part 2 Done

    cout<<"Corner IDs: ";
    for(int a=0;a<cornerIDs.size();a++) {
        cout<<cornerIDs.at(a) << " ";
    }                                                //Part 1 Output
    cout<<endl;
    cout<<"Water Roughness: "<<waterroughness<<endl; //Part 2 Output
    print_trimmed_grid();
    
    for(auto &pair: imagedict) {
        delete pair.second;
    }
    return 0;
}
