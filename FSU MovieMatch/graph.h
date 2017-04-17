/*
    graph.h
    Andrew J Wood
    COP 4530
 
    This is the header file for the graph API.  It defines the ALUGraph (Adjacency List Undirected)
    and the ALDGraph (Adjaceny List Directed).
 
    Note that the code is self-documenting.
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <list.h>
#include <vector.h>
#include <cstdlib>

namespace fsu {
    
    template < typename N >
    class ALUGraph
    {
    public:
        
        typedef N                                   Vertex;
        typedef typename fsu::List<Vertex>          SetType;
        typedef typename SetType::ConstIterator     AdjIterator;
        
        void    SetVrtxSize (N n);
        size_t  VrtxSize    () const;
        void    AddEdge     (Vertex from, Vertex to);
        bool    HasEdge     (Vertex from, Vertex to) const;
        size_t  EdgeSize    () const;
        size_t  OutDegree   (Vertex v) const;
        size_t  InDegree    (Vertex v) const;
        
        void    Clear       ();
        void    Dump        (std::ostream & os);
        void    Shuffle     ();
        
        AdjIterator Begin   (Vertex x) const;
        AdjIterator End     (Vertex x) const;
        
        ALUGraph            (); //constructor
        explicit ALUGraph   (N n); //constructor with no type converting allowed
        
    protected:
        
        fsu::Vector <SetType> al_; //vector containing the vertices
        
    }; //end class ALUGraph
    
    
    //Directed graph - derived from undirected graph
    template < typename N >
    class ALDGraph : public ALUGraph <N>
    {
    public:
        typedef N                                   Vertex;
        typedef typename ALUGraph<N>::SetType       SetType;
        typedef typename ALUGraph<N>::AdjIterator   AdjIterator;
        
        void    AddEdge     (Vertex from, Vertex to);
        size_t  EdgeSize    () const;
        size_t  InDegree    (Vertex v) const;
        void Reverse        (ALDGraph & d) const;
        
        ALDGraph            ();
        explicit ALDGraph   (N n);
        
    }; //end class ALDGraph
    
    
    //--
    //Undirected graph implementations
    //--
    
    template < typename N >
    void ALUGraph<N>::SetVrtxSize (N n)
    {
        al_.SetSize(n); //sets the underlying vector size
    }
    
    template < typename N >
    size_t ALUGraph<N>::VrtxSize() const
    {
        return al_.Size(); //returns number of vertices (size of vector)
    }
    
    template < typename N >
    void ALUGraph<N>::AddEdge(Vertex from, Vertex to)
    {
        al_[from].Insert(to); //adds "to" vertex to the corresponding list
        al_[to].Insert(from);
    }
    
    template < typename N >
    bool ALUGraph<N>::HasEdge (Vertex from, Vertex to) const
    {
        AdjIterator i = al_[from].Includes(to);
        if (i == al_[from].End())
            return 0; //not found
        return 1; //found
    }
    
    template < typename N >
    size_t ALUGraph<N>::EdgeSize() const
    {
        size_t esize = 0;
        for (Vertex v = 0; v < al_.Size(); ++v)
            esize += al_[v].Size();
        return (esize / 2); //since this is undirected
    }
    
    template < typename N >
    size_t ALUGraph<N>::OutDegree(Vertex v) const
    {
        return al_[v].Size(); //return size of list corresponding to vertex
    }
    
    template < typename N >
    size_t ALUGraph<N>::InDegree(Vertex v) const
    {
        return al_[v].Size(); //for undirected graphs, same as OutDegree
    }
    
    template < typename N >
    void ALUGraph<N>::Clear()
    {
        //loop through and clear all subsidiary structures
        for (Vertex v = 0; v < al_.Size(); ++v)
            al_[v].Clear(); //clear all lists
        al_.Clear(); //set vector size = 0
    }
    
    template < typename N >
    void ALUGraph<N>::Dump(std::ostream & os)
    {
        AdjIterator j;
        for (Vertex v = 0; v < VrtxSize(); ++v)
        {
            os << "[" << v << "]->";
            j = this->Begin(v);
            if (j != this->End(v))
            {
                os << *j;
                ++j;
            }
            for (; j != this->End(v); ++j)
            {
                os << ',' << *j;
            }
            os << '\n';
        }
    }
    
    template < typename N >
    void ALUGraph<N>::Shuffle()
    {
        for (Vertex v = 0; v < VrtxSize(); ++v)
            al_[v].Shuffle();
    }
    
    template < typename N >
    typename ALUGraph<N>::AdjIterator ALUGraph<N>::Begin (Vertex x) const
    {
        return al_[x].Begin(); //returns Begin list iterator
    }
    
    template < typename N >
    typename ALUGraph<N>::AdjIterator ALUGraph<N>::End (Vertex x) const
    {
        return al_[x].End(); //returns End list iterator
    }
    
    template < typename N >
    ALUGraph<N>::ALUGraph()
    {} //default constructor - nothign to be done here; vector will initialize to size 0
    
    template < typename N >
    ALUGraph<N>::ALUGraph(N n)
    {
        al_.SetSize(n); //sets vector to size n; indicates number of vertices in graph
    }
    
    
    // --
    //Directed graph implementations
    // --
    
    template < typename N>
    void ALDGraph<N>::AddEdge(Vertex from, Vertex to)
    {
        (this->al_)[from].Insert(to); //this needed since this is templated function
    }
    
    template < typename N >
    size_t ALDGraph<N>::EdgeSize() const
    {
        size_t esize = 0;
        for (Vertex v = 0; v < (this->al_).Size(); ++v)
            esize += ALUGraph<N>::al_[v].Size();
        return esize;
    }
    
    template < typename N >
    size_t ALDGraph<N>::InDegree (Vertex v) const
    {
        size_t indegree = 0;
        AdjIterator j;
        for (Vertex x = 0; x < this->VrtxSize(); ++x)
        {
            for (j = this->Begin(x); j != this->End(x); ++j)
            {
                if (v == *j) ++indegree;
            }
        }
        return indegree;
    }
    
    template < typename N >
    void ALDGraph<N>::Reverse (ALDGraph & d) const
    {
        //the graph d gets created as a reverse of *this
        d.Clear(); //clears d - empty graph
        d.SetVrtxSize(this->VrtxSize()); //sets d to size of *this
        
        for (Vertex x = 0; x < this->VrtxSize(); ++x)
        {
            for (AdjIterator j = this->Begin(x); j != this->End(x); ++j) //go through list
            {
                d.AddEdge(*j, x); //add edge from *j to x
            }
        }
        
    }
    
    template < typename N >
    ALDGraph<N>::ALDGraph () : ALUGraph<N>() //default constructor
    {}
    
    template < typename N >
    ALDGraph<N>::ALDGraph(N n) : ALUGraph<N> (n)
    {}
    
}//end namespace fsu

#endif /* GRAPH_H */
