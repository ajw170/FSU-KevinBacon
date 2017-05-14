/*
    moviematch.h
    Andrew J Wood
    COP4530
 
    Defines and implements the Kevin Bacon - "Six Degrees of Separation" game.
 
    The user will be able to determine any selected actor's Kevin Bacon number!
 
    The following technologies are used in the implementation:
        -Graphs
        -Graph Search and Survey
        -Path Computation in Graphs
        -Associative Arrays [implemented via hash tables]
        -Generic sort algorithms (specifically, heap sort)
        -Generic binary search
 
    Note that the code is self-documenting.
 
 */

#ifndef MOVIEMATCH_H
#define MOVIEMATCH_H

#include <xstring.h>
#include <cstdlib>
#include <graph.h>
#include <bfsurvey.h>
#include <vector.h>
#include <hashclasses.h>
#include <hashtbl.h>
#include <graph_util.h>
#include <survey_util.h>
#include <list.h>
#include <cctype>
#include <genalg.h>
#include <gheap.h>
#include <gbsearch.h>


//class for sorting case insensitve strings
class CaseInsensitiveLessThan
{
public:
    bool operator () (const fsu::String& s1, const fsu::String& s2)
    {
        size_t i = 0;
        for (i = 0; i < s1.Size() && i < s2.Size(); ++i)
        {
            if (tolower(s1[i]) < tolower(s2[i])) return 1;
            if (tolower(s1[i]) > tolower(s2[i])) return 0;
        }
        if (s1.Size() < s2.Size()) return 1;
        return 0;
    }
};

//The main MovieMatch class
class MovieMatch
{
public:
    
    //terminology support
    typedef size_t                              Vertex;
    typedef fsu::String                         Name;
    typedef fsu::ALUGraph<Vertex>               Graph;
    typedef fsu::BFSurvey<Graph>                BFS;
    typedef hashclass::KISS<Name>               Hash;
    typedef fsu::HashTable<Name,Vertex,Hash>    AA; //associative array
    typedef fsu::Vector<Name>                   Vector; //vector of strings
    typedef fsu::List<Vertex>                   List; //list of vertices
    
            MovieMatch ();          //default constructor
    bool    Load    (const char * filename);
    bool    Init    (const char * actor);
    void    Shuffle ();
    long    MovieDistance (const char * actor);
    void    ShowPath (std::ostream & os) const;
    void    ShowStar (Name name, std::ostream & os) const;
    void    Hint (Name name, std::ostream & os, size_t size) const;
    void    Dump (std::ostream & os) const;
    
private:
    
    static void Line (std::istream & is, Vector & movie);  //helper read function
    bool isMovie (Vertex v);                        //takes a vertex and determines if it is a movie
    
    Graph   g_; //the bipartite graph connecting actors with movies
    Vector  name_; //the vector mapping vertex numbers to names
    Vector  hint_; //the hint vector used
    AA      vrtx_; //the associatve array mappint names to vertex numbers
    BFS     bfs_; //the breadth first survey
    
    Name    baseActor_; //holds the base actor's name
    List    path_; //holds the path from specified vertex to base
    
}; //end class MovieMatch

//default constructor - only initial object is created
MovieMatch::MovieMatch() : g_(), name_(), hint_(), vrtx_(), bfs_(g_), baseActor_()
{}

bool MovieMatch::Load (const char * filename)
{
    std::cout << " Loading database " << filename << " (first read) ...";
    
    //read first line of file
    fsu::Vector<Name> inFileVector;
    Vertex dummyVertexNum = 0;          //General purpose vertex
    Vertex vertexNum = 0;               //used to insert values
    bool isThere = 0;                   //Used to test presence in AA
    size_t movieCount = 0;
    size_t actorCount = 0;
    size_t numBuckets = 100;            //used to optimize hash table load
    
    std::ifstream inFile(filename, std::ios::in);
    if (!inFile)
        return 0; //file failed to open
    inFile.seekg(0); //ensure pointer is at 0th byte of file
    
    
    //build AA and name Vector
    while (!(inFile.eof()))
    {
        Line(inFile, inFileVector); //reads line with movie and actor information
        if (inFileVector.Size() > 0) //if it's not a blank line
            ++movieCount; //increment movie count (each line is a movie)
        
        //check to see if vertex is already in graph
        //loop through vector and attempt to insert names if they don't already exist
        for (size_t i = 0; i < inFileVector.Size(); ++i)
        {
            isThere = vrtx_.Retrieve(inFileVector[i],dummyVertexNum); //checks presence of vertex name
            if (!isThere) //if vertex doesn't already exist
            {
                vrtx_[inFileVector[i]] = vertexNum; //adds name to AA with specified vertex number
                name_.PushBack(inFileVector[i]);    //adds name to vector
                hint_.PushBack(inFileVector[i]);    //similar to name, but will be sorted in Init()
                ++actorCount;
                ++vertexNum; //increments vertex number
            }
        }
        
        //HashTable optimization Note: "actorCount" is number of elements inserted into symbol tables
        if (actorCount > (2 * numBuckets))
        {
            numBuckets *= 4; //double size of hash table
            vrtx_.Rehash(numBuckets);
        }
    }
    
    //final optimization of hash table
    vrtx_.Rehash(actorCount);
    
    //fix actor count to subtract out movie Count
    actorCount -= movieCount;
    
    //add edges to graph based on file
    inFile.clear(); //clear bad state after EOF
    inFile.seekg(0); //reposition pointer to beginning of file
    
    std::cout << "(second read) ... ";
    
    g_.SetVrtxSize(vrtx_.Size()); //sets number of graph vertices in the HashTable
    
    while (!(inFile.eof()))
    {
        Line(inFile, inFileVector); //read line from file
        for (size_t i = 1; i < inFileVector.Size(); ++i)
        {
            //adds edge from movie in position 0 to actor in position i
            g_.AddEdge(vrtx_[inFileVector[0]], vrtx_[inFileVector[i]]);
        }
    }
    
    inFile.close();
    
    std::cout << "done.\n ";
    std::cout << movieCount << " movies and " << actorCount << " actors read from " << filename << "\n";
    
    return 1; //successful
}


//Initializes the BFS object with the actor as the start point
bool MovieMatch::Init (const char * actor)
{
    CaseInsensitiveLessThan pred_; //declares predicate object
    fsu::g_heap_sort(hint_.Begin(),hint_.End(), pred_); //sorts the hint array
    
    //determine if the actor is in the database
    Vertex v;
    bool isHere = vrtx_.Retrieve(actor,v); //if successful, vertex number will be in v

    if (!isHere)
    {
        std::cerr << " ** Init: " << actor << " is not in the database\n";
        return 0; //failure
    }
    else if (isMovie(v))
    {
        std::cerr << " ** Init: " << actor << " is a movie\n";
        return 0; //failure
    }
    else
    {
        baseActor_ = actor; //sets base actor in MM object
        bfs_.Reset(); //resets BFS object with up-do-date graph information
        bfs_.Search(v); //search graph with vertex v as base

        return 1;
    }
}


void MovieMatch::Shuffle()
{
    g_.Shuffle();
    bfs_.Reset();
    bfs_.Search(vrtx_[baseActor_]);
}


long MovieMatch::MovieDistance(const char * actor)
{
    //-3, -2, or -1 or actual movie distance
    Vertex v;
    bool isHere = vrtx_.Retrieve(actor, v); //if successful, vertex number will be in v

    if (!isHere)
    {
        return -3; //name is not in database
    }
    else if (bfs_.Color()[v] != 'b') //if the actor is unreachable from base
    {
        return -2;
    }
    else if (isMovie(v))
    {
        return -1;
    }
    else //the actor is reachable, compute distance and store path
    {
        long movieDistance = bfs_.Distance()[v] / 2; //computed directly from bfs_'s distance vector

        path_.Clear(); //clear any pre-existing path
        path_.PushBack(v); //push actor vertex onto path_
        
        //note: the base actor's parent will be null_
        while (bfs_.Parent()[v] != bfs_.NullVertex())
        {
            path_.PushBack(bfs_.Parent()[v]);
            v = bfs_.Parent()[v];
        }

        return movieDistance;
    }
}


void MovieMatch::ShowPath(std::ostream & os) const
{
    size_t counter = 0;
    
    List::ConstIterator i;
    i = path_.Begin();
    
    os << "\n";
    for (i = path_.Begin(); i != path_.End(); ++i)
    {
        if ((counter % 2) == 1)
            os << "   | ";
        else
            os << " ";
        os << name_[*i] << "\n";
        ++counter;
    }
    os << "\n";
}


void MovieMatch::ShowStar(Name name, std::ostream & os) const
{
    Graph::AdjIterator i;
    Vertex v = vrtx_[name]; //determines vertex number of the star name; note: protected in const environment
    
    Vector sortedStar;
    for (i = g_.Begin(v); i != g_.End(v); ++i)
    {
        sortedStar.PushBack(name_[*i]); //push names onto list
    }
    
    CaseInsensitiveLessThan pred_;
    fsu::g_heap_sort(sortedStar.Begin(), sortedStar.End(), pred_); //sorts the vector
    
    os << "\n ";
    os << name << "\n";
    
    for (Vector::ConstIterator i = sortedStar.Begin(); i != sortedStar.End(); ++i)
    {
        os << "   | " << *i << "\n";
    }
    
    os << "\n\n";
}



void MovieMatch::Hint (Name name, std::ostream & os, size_t size = 6) const
{
    CaseInsensitiveLessThan pred_;
    size_t truncSize = size;
    size_t nameSize = name.Size();
    
    if (nameSize < size)
        truncSize = nameSize;
    
    Name trunc; //creates string "trunc"
    Name trunczz; //creates string "trunczz"
    
    char charString[truncSize+1]; //make room for null character
    for (size_t i = 0; i < truncSize; ++i)
    {
        charString[i] = name.Element(i);
    }
    charString[truncSize] = '\0';
    trunc.Wrap(charString);
    
    
    char charStringZ[truncSize+3]; //make room for zz plus null character
    for (size_t i = 0; i < (truncSize); ++i)
    {
        charStringZ[i] = name.Element(i);
    }
    charStringZ[truncSize] = 'z';
    charStringZ[truncSize+1] = 'z';
    charStringZ[truncSize+2] = '\0';
    trunczz.Wrap(charStringZ);
    
    
    Vector::ConstIterator fullHintBegin = hint_.Begin();
    Vector::ConstIterator fullHintEnd   = hint_.End();
    
    
    Vector::ConstIterator hintBegin = fsu::g_lower_bound(fullHintBegin, fullHintEnd, trunc, pred_);
    Vector::ConstIterator hintEnd   = fsu::g_upper_bound(fullHintBegin, fullHintEnd, trunczz, pred_);

    
    //move hintBegin iterator back by 2, if possible
    for (size_t count = 0; count < 2; ++count)
    {
        if (hintBegin != fullHintBegin)
            --hintBegin;
    }
    
    
    //move hintEnd iterator forward by 2, is possible
    for (size_t count = 0; count < 2; ++count)
    {
        if (hintEnd != fullHintEnd)
            ++hintEnd;
    }
    
    
    Vector::ConstIterator h;
    for (h = hintBegin; h != hintEnd; ++h)
    {
        std::cout << *h << "\n";
    }
    
}

void MovieMatch::Dump (std::ostream & os) const
{
    ShowAL(g_,os);
    WriteData(bfs_,os);
    vrtx_.Dump(os);
    for (size_t i = 0; i < name_.Size(); ++i)
    {
        os << "name_[" << i << "] = " << name_[i] << '\t';
        os << "vrtx_[" << name_[i] << "] = " << vrtx_[name_[i]] << '\n';
    }
    vrtx_.Analysis(std::cout);
}

void MovieMatch::Line (std::istream& is, fsu::Vector<Name>& movie)
{
    movie.Clear();
    fsu::String line;
    char delim = '/';
    line.GetLine(is);
    char* name_buffer = new char [1+line.Size()];
    size_t pos = 0, next = 0;
    while (pos < line.Size())
    {
        next = line.Position(delim,pos);
        for (size_t i = pos; i < next; ++i)
        {
            name_buffer[i - pos]= line[i];
        }
        name_buffer[next-pos] = '\0';
        movie.PushBack(name_buffer);
        pos = ++next; // skip delimiter
    }
    delete [] name_buffer;
}

bool MovieMatch::isMovie(Vertex v)
{
    fsu::String s(name_[v]); //instantiates string with name corresponding to vertex
    size_t stringSize = s.Length(); //gets lengh of string
    //determines if last part of string is a year
    if (s[stringSize -1] != ')')
    {
        return 0; //not a movie
    }
    else if (   isdigit(s[stringSize-2]) &&
                isdigit(s[stringSize-3]) &&
                isdigit(s[stringSize-4]) &&
                isdigit(s[stringSize-5]) &&
                (s[stringSize-6] == '(')
            )
    {
        return 1; //title ends in a year, is a movie
    }
    else
    {
        return 0; //not a movie
    }
}

#endif /* MOVIEMATCH_H */
