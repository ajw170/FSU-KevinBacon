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
    void    Hint (Name name, std::ostream & os) const;
    void    Dump (std::ostream & os) const;
    
private:
    
    static void Line (std::istream & is, Vector & movie);  //helper read function
    bool isMovie (Vertex v);                        //takes a vertex and determines if it is a movie
    
    Graph   g_; //the bipartite graph connecting actors with movies
    Vector  name_; //the vector mapping vertex numbers to names
    AA      vrtx_; //the associatve array mappint names to vertex numbers
    BFS     bfs_; //the breadth first survey
    
    Name    baseActor_; //holds the base actor's name
    List    path_; //holds the path from specified vertex to base
    
}; //end class MovieMatch

//default constructor - only initial object is created
MovieMatch::MovieMatch() : g_(), name_(), vrtx_(), bfs_(g_), baseActor_()
{}

bool MovieMatch::Load (const char * filename)
{
    //read first line of file
    fsu::Vector<Name> inFileVector;
    Vertex dummyVertexNum = 0;               //General purpose vertex
    Vertex vertexNum = 0;               //used to insert values
    bool isThere = 0;                   //Used to test presence in AA
    size_t movieCount = 0;
    size_t actorCount = 0;
    
    std::ifstream inFile(filename, std::ios::in);
    if (!inFile)
        return 0; //file failed to open
    inFile.seekg(0); //ensure pointer is at 0th byte of file
    
    std::cout << " Loading database " << filename << " (first read) ...";
    
    //build AA and name Vector
    while (!(inFile.eof()))
    {
        Line(inFile, inFileVector); //reads line with movie and actor information
        //inFileVector.Dump(std::cout);
        if (inFileVector.Size() > 0) //if it's not a blank line
            ++movieCount; //increment movie count (each line is a movie)
        
        //check to see if vertex is already in graph
        //loop through vector and attempt to insert names if they don't already exist
        for (size_t i = 0; i < inFileVector.Size(); ++i)
        {
            //std::cout << "InFile Vector Read: " << inFileVector[i] << "\n";
            
            isThere = vrtx_.Retrieve(inFileVector[i],dummyVertexNum); //checks presence of vertex name
            //std::cout << "Was there? : " << isThere << "\n";
            
            
            
            if (!isThere) //if vertex doesn't already exist
            {
                //std::cout << "The vertex number is " << vertexNum << "\n";
                vrtx_[inFileVector[i]] = vertexNum; //adds name to AA with specified vertex number
                
                name_.PushBack(inFileVector[i]);    //adds name to vector
               // std::cout << "After loading into the AA, the vrtx_[" << inFileVector[i] << "] is " << vrtx_[inFileVector[i]] << "\n";
                //std::cout << "The name that was just pushed onto the name_ vector is " << name_.Back() << "\n";
            
                ++actorCount;
                ++vertexNum; //increments vertex number
            }
        }
    }
    //fix actor count to subtract out movie Count
    actorCount -= movieCount;
    
    //add edges to graph based on file
    inFile.clear(); //clear bad state after EOF
    inFile.seekg(0); //reposition pointer to beginning of file
    
    g_.SetVrtxSize(vrtx_.Size()); //sets number of graph vertices in the HashTable
    
    std::cout << "(second read) ... ";
    
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
    std::cout << movieCount << " movies and " << actorCount << " actors read from " << filename;
    
    
    
    //std::cout << "Dumps of everything so far: \n\n";
    
   // name_.Dump(std::cout);
   // vrtx_.Dump(std::cout);
    
    
    
    
    
    
    
    return 1; //successful
}


//Initializes the BFS object with the actor as the start point
bool MovieMatch::Init (const char * actor)
{
    //determine if the actor is in the database
    Vertex v;
    bool isHere = vrtx_.Retrieve(actor,v); //if successful, vertex number will be in v
    //std::cout << "Initialzied with actor " << actor << " and vertex pulled is " << v;
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
        //std::cout << "Searching graph with vertex v = " << v << " whose name is " << name_[v];
        //(bfs_.Color()).Dump(std::cout);
        bfs_.Search(v); //search graph with vertex v as base
        //(bfs_.Parent()).Dump(std::cout);
        //(bfs_.Color()).Dump(std::cout);
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
    std::cout << "The actor to find is " << actor << "\n";
    bool isHere = vrtx_.Retrieve(actor, v); //if successful, vertex number will be in v
    //std::cout << "The vertex retreived is " << v << " with name " << name_[v];
    if (!isHere)
    {
        return -3; //name is not in database
    }
    else if (bfs_.Color()[v] != 'b') //if the actor is unreachable from base
    {
        //(bfs_.Color()).Dump(std::cout);
        return -2;
    }
    else if (isMovie(v))
    {
        return -1;
    }
    else //the actor is reachable, compute distance and store path
    {
        std::cout << "The vertex is: " << v;
        long movieDistance = bfs_.Distance()[v] / 2; //computed directly from bfs_'s distance vector
        //(bfs_.Distance()).Dump(std::cout);
        
        
        path_.Clear(); //clear any pre-existing path
        path_.PushBack(v); //push actor vertex onto path_
        
        //note: the base actor's parent will be null_
        while (bfs_.Parent()[v] != bfs_.NullVertex())
        {
            path_.PushBack(bfs_.Parent()[v]);
            v = bfs_.Parent()[v];
        }
        //std::cout << "The path vector is : \n";
        //path_.Dump(std::cout);
        
        return movieDistance;
    }
}


void MovieMatch::ShowPath(std::ostream & os) const
{
    List::ConstIterator i;
    size_t counter = 0;
    i = path_.Begin();
    //std::cout << "Begin vertex is: " << *i << " with name " << name_[*i];
    
    
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
    
    os << "\n ";
    os << name << "\n";
    
    for (i = g_.Begin(v); i != g_.End(v); ++i)
    {
        os << "   | " << name_[*i] << "\n";
    }
    
    os << "\n\n";
}



void MovieMatch::Hint (Name name, std::ostream & os) const
{
    os << "\nThis is a placeholder.\n";
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
