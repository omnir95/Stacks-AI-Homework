//Omar Natour
//tidy
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
using namespace std;

class cl_poped;
class less_than;
class cl_state;
class cl_expansion;
class cl_heap;


/*
the following function will take from the main the info about all stacks in each initial and final cases
this function will create states and responsible for calling defferent functions
*/
void doTheWork ( vector<char> *initStcksArr , int nbStcks , vector<char> **golStcksArr , int nbGolStcks );
void swaping ( cl_state &state1 , cl_state &state2 );

/*following class helps to speed up the operation of adding an poping in stacks*/
class cl_poped {
public:
	char charecter;
	int stackIndex;
	cl_poped ();
	cl_poped (char , int);
};
cl_poped::cl_poped (){
}
cl_poped::cl_poped (char ch , int ind){
	stackIndex = ind;
	charecter = ch;
}

/*this class will represent each state (order of stacks)
contains the heurestic of it , cost , summation of them and the move from it's parent
*/
class cl_state {
public:
	int heuristic;
	int cost;
	int sum; // cost + heuristic
	cl_poped src , dist; // src : the charecter moved , dist : the charecter that src will placed above it .('-' means floor)
	static int nbStacksInGoal;
	static vector<char> *goalStack; //all states need to see it in which they can evaluate thair heuristics
	vector<vector<char>> stacksArr; // this will contain the order of stacks in current state
	string move; // contains all steps satrting from initial state tell the current state.

public:
	//next constructor creates a state from another one by moving a letter
	cl_state ( cl_state &parentP , cl_poped srcP , cl_poped distP );
	//next constructor will create the initial state.
	cl_state ( vector<char> *initStcksArray , int initStcksArraySize , vector<char> *golStcksArray , int golStcksArraySize );
	cl_state ();
	//next will find the correct blocks places number
	int findMaxSubset ();
	int heuristicEval ();
	int sumEval ();
	int getSum ()const;
	void printTrace ();
	//this will make the movement from src to dist in stacks
	void makeMove ();
	void compressStacks (int);
	bool equal ( cl_state &toCompare );
};
vector<char>* cl_state::goalStack = NULL;
int cl_state::nbStacksInGoal = 0;
cl_state::cl_state (){}
cl_state::cl_state ( cl_state &parentP , cl_poped srcP , cl_poped distP ){
	if (distP.charecter != '-')
		move = move + "move " + srcP.charecter + " to " + distP.charecter;
	else
		move = move + "move " + srcP.charecter + " to floor";

	src = srcP;
	dist = distP;
	move = parentP.move + "\n" + move;

	cost = 1 + parentP.cost;
	stacksArr = parentP.stacksArr;
	makeMove ();

	heuristicEval ();//evaluate current heuristic
	sumEval ();
}
cl_state::cl_state ( vector<char> *initStcksArray , int initStcksArraySize , vector<char> *golStcksArray , int golStcksArraySize ){
	cost = 0;
	nbStacksInGoal = golStcksArraySize;
	move = "";
	for (int i = 0; i < initStcksArraySize; i++)
		stacksArr.push_back ( initStcksArray[i] );
	goalStack = golStcksArray;
	heuristicEval ();
	sumEval ();
}


int cl_state::findMaxSubset (){
	int sumMax = 0;
	for (int j = 0; j < nbStacksInGoal; j++) {
		int i;
		int max = 0;
		for (i = 0; i < stacksArr.size(); i++) {
			int ctr ;
			for (ctr = 0; ctr < stacksArr[i].size () && ctr < goalStack[j].size () && (stacksArr[i].at ( ctr ) == goalStack[j].at ( ctr )); ctr++);			
			max += ctr;
		}
		sumMax += max;
	}
	return sumMax;
}
int cl_state::heuristicEval (){

	int maxSub = findMaxSubset ();
	int nbAllBlocks = 0;
	for (int i = 0; i < nbStacksInGoal; i++)
		nbAllBlocks += goalStack[i].size ();
	heuristic = nbAllBlocks - maxSub;

	return 0;
}
int cl_state::sumEval (){
	sum = heuristic + cost;
	return 0;
}
int cl_state::getSum ()const{
	return sum;
}
void cl_state::printTrace (){
	cout << move << endl;
}
//following function will differ the current state stacks order from it's parent's
void cl_state::makeMove (){
	
	if (dist.charecter == '-') { 
		vector<char>temp;
		temp.push_back ( src.charecter );
		stacksArr.push_back ( temp );
		stacksArr[src.stackIndex].pop_back ();
		compressStacks ( src.stackIndex );
		return;
	}
	stacksArr[dist.stackIndex].push_back ( src.charecter );
	stacksArr[src.stackIndex].pop_back ();
	compressStacks (src.stackIndex);
}

//the following will remove the empty stacs >> increases spead in checking for charecters !
void cl_state::compressStacks (int index){
	
	if (stacksArr[index].empty ()) {
		stacksArr[index] = stacksArr.back ();
		stacksArr.pop_back ();
		return;
	}
}
bool cl_state::equal ( cl_state &toCompare ){
	if (stacksArr.size () != toCompare.stacksArr.size ())
		return false;

	for (int i = 0; i < stacksArr.size(); i++) {

		int flag = 0;
		for (int j = 0; j < stacksArr.size (); j++) 
			if (stacksArr[i] == toCompare.stacksArr[j]) {
			flag = 1;
			break;
			}

		if (!flag)
			return false;
	}
	return true;
}

//my own implementation for heap , because the standered library doesn't contain removing from the meddle of the heap function.
class cl_heap : public vector<cl_state> {

public:
	cl_state retrieve_head_heap ();
	void push_back_heap ( cl_state pushed );
	void erasing_at_heap ( int index );

};
cl_state cl_heap::retrieve_head_heap (){
	cl_state temp;
	if (size () <= 0)return temp;
	if (size () == 1) {
		temp = at ( size () - 1 );
		pop_back ();
		return temp;
	}

	swaping ( (*this)[0] , (*this)[size () - 1] );
	temp = at ( size () - 1 );
	pop_back ();
	int leastChildIndex;
	int index = 0;
	int leftIndex = index * 2 + 1;
	int rightIndex = index * 2 + 2;
	while (leftIndex < size () || rightIndex < size ()) {

		if (leftIndex >= size ())
			leastChildIndex = rightIndex;
		else
			if (rightIndex >= size ())
				leastChildIndex = leftIndex;
			else
				if (at ( leftIndex ).sum < at ( rightIndex ).sum)
					leastChildIndex = leftIndex;
				else
					leastChildIndex = rightIndex;

		if (at ( leastChildIndex ).sum < at ( index ).sum) {

			swaping ( (*this)[index] , (*this)[leastChildIndex] );
			index = leastChildIndex;
			rightIndex = index * 2 + 2;
			leftIndex = index * 2 + 1;
		}
		else
			return temp;
	}
	return temp;

}
void cl_heap::erasing_at_heap ( int indexP ){

	if (size () <= 0)
		return;
	if (size () == 1) {
		pop_back ();
		return;
	}
	swaping ( (*this)[indexP] , (*this)[size () - 1] );
	pop_back ();
	int leastChildIndex;
	int index = indexP;
	int leftIndex = index * 2 + 1;
	int rightIndex = index * 2 + 2;
	while (leftIndex < size () || rightIndex < size ()) {

		if (leftIndex >= size ())
			leastChildIndex = rightIndex;
		else
			if (rightIndex >= size ())
				leastChildIndex = leftIndex;
			else
				if (at ( leftIndex ).sum < at ( rightIndex ).sum)
					leastChildIndex = leftIndex;
				else
					leastChildIndex = rightIndex;

		if (at ( leastChildIndex ).sum < at ( index ).sum) {

			swaping ( (*this)[index] , (*this)[leastChildIndex] );
			index = leastChildIndex;
			rightIndex = index * 2 + 2;
			leftIndex = index * 2 + 1;
		}
		else
			return;
	}

}
void cl_heap::push_back_heap ( cl_state pushed ){
	push_back ( pushed );
	int index = size () - 1;
	while (index > 0 && at ( index ).sum < at ( (index - 1) / 2 ).sum) {
		cl_state st1 , st2;
		swaping ( (*this)[index] , (*this)[(index - 1) / 2] );
		index = (index - 1) / 2;
	}
}


class cl_expansion {
public:

	cl_heap setOfStates; // heap of states which need to be expanded "open states"
	vector<cl_state> setOfClosed; // normal vector of already expanded states 
	int nbExpanded;

	//this constructor will get the initial state and start all the work.
	cl_expansion ( cl_state initState );

	//this will add as a heap to the setOfStates vector
	void addSorted ( cl_state &state );

	//this will check all teh open states if there is an already found state with the same order of blocks it keeps the better one
	// if there is one that is the same of it then the newly found will be deleted as in A* algorithm.
	void keepTheBest ( cl_state newlyGot );

	//this will get the state just removed from the open states and check all possible movements, it also calls the best state chooser function (keepTheBest)
	bool expandAll ( cl_state toExpand );
};

cl_expansion::cl_expansion ( cl_state initState ){
	nbExpanded = 0;
	addSorted ( initState );
	while (!setOfStates.empty ()) {
		nbExpanded++;
		cl_state temp = setOfStates.retrieve_head_heap ();

		setOfClosed.push_back ( temp );

		if (!expandAll ( temp )) {
			cout << "found the goal : " << endl;
			temp.printTrace ();

			setOfStates.clear ();
			setOfClosed.clear ();
			return;
		}
	}
	cout << "no goal" << endl;
}
void cl_expansion::addSorted ( cl_state &state ){

	setOfStates.push_back_heap ( state );

}
void cl_expansion::keepTheBest ( cl_state newlyGot ){

	for (int i = 0; i < setOfStates.size (); i++) 
		
		if (newlyGot.equal ( setOfStates.at ( i ) )) {
			if (newlyGot.getSum () < setOfStates.at ( i ).getSum ()) {
				setOfStates.erasing_at_heap ( i );
				addSorted ( newlyGot );				
			}
			return;
		}

	for (int i = 0; i < setOfClosed.size (); i++)
		if (newlyGot.equal ( setOfClosed.at ( i ) )) 
		return;
		
	addSorted ( newlyGot );
}
bool cl_expansion::expandAll ( cl_state toExpand ){ // return value > true : keep expnding , false : stop. (I found the goal)

	if (toExpand.heuristic == 0)
		return false;

	for (int i = 0; i < toExpand.stacksArr.size (); i++) {

		cl_poped from(toExpand.stacksArr[i].back() , i);
		cl_poped to ( '-' , -1 );
		cl_state tempFloor ( toExpand , from , to ); 
		keepTheBest ( tempFloor );

		for (int j = 0; j < toExpand.stacksArr.size(); j++)
			if (j != i ) {

			cl_poped toFor ( toExpand.stacksArr[j].back () , j );
			cl_state temp ( toExpand , from , toFor );
			keepTheBest ( temp );
			}		
		}
	return true;

}


int main ( void ){

	int nbStcks , nbGolStcks;
	float deff;
	vector<char> *initStcksArr;
	vector<char> *golStcksArr;
	time_t initialTime , finalTime;
	cout << "Specify initial state" << endl << "Enter the number of initial stacks:";
	cin >> nbStcks;
	initStcksArr = new vector<char>[nbStcks];

	cout << "Enter the vector(s) as strings (each on a separate line)" << endl;
	for (int i = 0; i < nbStcks; i++) {
		string got;
		cin >> got;
		for (int j = got.length () - 1; j >= 0; j--)
			initStcksArr[i].push_back ( got.at ( j ) );
	}

	// now the goal.

	cout << "Specify goal state\n" << "Enter the number of goal stacks:";
	cin >> nbGolStcks;
	golStcksArr = new vector<char>[nbGolStcks];

	cout << "Enter the vector(s) (each on a separate line)" << endl;
	for (int i = 0; i < nbGolStcks; i++) {
		string got;
		cin >> got;
		for (int j = got.length () - 1; j >= 0; j--)
			golStcksArr[i].push_back ( got.at ( j ) );
	}
	initialTime = clock ();

	doTheWork ( initStcksArr , nbStcks , &golStcksArr , nbGolStcks );

	finalTime = clock ();
	deff = (float)finalTime - (float)initialTime;
	cout << "Elapsed time :" << (float)deff / (float)1000 << " Sec." << endl;

}
void doTheWork ( vector<char> *initStcksArr , int nbStcks , vector<char> **golStcksArr , int nbGolStcks ){
	cout << "Working , please wait..." << endl;
	cl_state initState ( initStcksArr , nbStcks , *golStcksArr , nbGolStcks );
	golStcksArr = NULL; // to prevent multipointing 
	cl_expansion expansion ( initState );
	cout << "States Expanded : " << expansion.nbExpanded << endl;
}
void swaping ( cl_state &state1 , cl_state &state2 ){
	cl_state temp;
	temp = state1;
	state1 = state2;
	state2 = temp;

}
