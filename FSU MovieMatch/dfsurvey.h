/*
 dfsurvey.h
 Andrew J Wood
 COP 4530
 
 This is the header file for the depth-first survey.  It defines and implements the
 DFSurvery class which is used to do a depth-first search of a given graph.
 
 Note that the code is self-documenting.
 */

#ifndef DFSURVEY_H
#define DFSURVEY_H

#include <vector.h>
#include <deque.h>

namespace fsu {

    template < class G >
    class DFSurvey
    {
        
    public:
        
        typedef G                           Graph;
        typedef typename Graph::Vertex      Vertex;
        typedef typename Graph::AdjIterator AdjIterator;
        
        DFSurvey        (const Graph & g);
        DFSurvey        (const Graph & g, Vertex start);
        void Search     ();
        void Search     (Vertex v);
        void Reset      ();
        void Reset      (Vertex start);
        
    private:
        
        const Graph &                       g_;
        Vertex                              start_;
        size_t                              time_;
        size_t                              forever_;
        Vertex                              null_;
        
        AdjIterator NextNeighbor(Vertex x); //returns iterator to next unvisited neighbor of x
        
        fsu::Vector<Vertex>                 dtime_; //discovery time
        fsu::Vector<Vertex>                 ftime_; //finishing time, up to 2|V|
        fsu::Vector<Vertex>                 parent_; //for DFS tree
        fsu::Vector<char>                   color_;
        fsu::Vector<AdjIterator>            neighbor_; //vector of list iterators (points to neighbor)
        fsu::Deque<Vertex>                  conQ_; //the control stack
        
    public:
        
        const fsu::Vector<Vertex>&   DTime    () const {return dtime_;}
        const fsu::Vector<Vertex>&   FTime    () const {return ftime_;}
        const fsu::Vector<Vertex>&   Parent   () const {return parent_;}
        const fsu::Vector<char>&     Color    () const {return color_;}
        
        size_t VrtxSize () const  {return g_.VrtxSize();}
        size_t EdgeSize () const  {return g_.EdgeSize();}
        
        size_t InfiniteTime     () const {return forever_;}
        Vertex NullVertex       () const {return null_;}
        
        //developer helper methods
        bool traceQue;
        void ShowQueSetup (std::ostream& os) const;
        void ShowQue      (std::ostream& os) const;
        
        
    }; //end class DFSurvey
    
    //----
    //DFSurvery Implementations
    //----
    
    template < class G >
    DFSurvey<G>::DFSurvey (const Graph & g)
    :   g_(g), start_(0), time_(0), forever_(2*g_.VrtxSize()), null_((Vertex)g_.VrtxSize()),
        dtime_      (g_.VrtxSize(), forever_),
        ftime_      (g_.VrtxSize(), forever_),
        parent_     (g_.VrtxSize(), null_),
        color_      (g_.VrtxSize(), 'w'),
        neighbor_   (g_.VrtxSize()),
        conQ_()
    {
        for (Vertex x = 0; x < g_.VrtxSize(); ++x)
            neighbor_[x] = g_.Begin(x);
    }
    
    template < class G >
    DFSurvey<G>::DFSurvey (const Graph & g, Vertex start)
    :   g_(g), start_(start), time_(0), forever_(2*g_.VrtxSize()), null_((Vertex)g_.VrtxSize()),
    dtime_      (g_.VrtxSize(), forever_),
    ftime_      (g_.VrtxSize(), forever_),
    parent_     (g_.VrtxSize(), null_),
    color_      (g_.VrtxSize(), 'w'),
    neighbor_   (g_.VrtxSize()),
    conQ_()
    {
        for (Vertex x = 0; x < g_.VrtxSize(); ++x)
            neighbor_[x] = g_.Begin(x);
    }
    
    template < class G >
    void DFSurvey<G>::Search()
    {
        Reset();
        if (traceQue)
        {
            ShowQueSetup(std::cout);
            ShowQue(std::cout);
        }
        for (Vertex v = start_; v < g_.VrtxSize(); ++v)
        {
            if (color_[v] == 'w')
                Search(v);
        }
        for (Vertex v = 0; v < start_; ++v)
        {
            if (color_[v] == 'w')
                Search(v);
        }
    }
    
    template < class G >
    void DFSurvey<G>::Search(Vertex v)
    {
        dtime_[v] = time_++;
        conQ_.PushBack(v);
        if (traceQue)
            ShowQue(std::cout);
        color_[v] = 'g';
        Vertex top;
        AdjIterator i;
        while (!conQ_.Empty())
        {
            top = conQ_.Back();
            i = NextNeighbor(top);
            if (i != g_.End(top))
            {
                dtime_[*i] = time_++;
                conQ_.PushBack(*i);
                if (traceQue)
                    ShowQue(std::cout);
                parent_[*i] = top;
                color_[*i] = 'g';
            }
            else
            {
                conQ_.PopBack();
                if (traceQue)
                    ShowQue(std::cout);
                color_[top] = 'b';
                ftime_[top] = time_++;
            }
        }
    }
    
    template < class G >
    void DFSurvey<G>::Reset()
    {
        time_ = 0;
        conQ_.Clear();
        if (color_.Size() != g_.VrtxSize()) //changed vertex size
        {
            forever_    = 2*g_.VrtxSize();
            null_       = (Vertex)g_.VrtxSize();
            dtime_.SetSize (g_.VrtxSize(), forever_);
            ftime_.SetSize (g_.VrtxSize(), forever_);
            parent_.SetSize (g_.VrtxSize(), null_);
            color_.SetSize (g_.VrtxSize(), 'w');
            neighbor_.SetSize(g_.VrtxSize());
            for (Vertex x = 0; x < g_.VrtxSize(); ++x)
                neighbor_[x] = g_.Begin(x);
        }
        else
        {
            for (Vertex x = 0; x < g_.VrtxSize(); ++x)
            {
                dtime_[x] = forever_;
                ftime_[x] = forever_;
                parent_[x] = null_;
                color_[x] = 'w';
                neighbor_[x] = g_.Begin(x);
            }
        }
    }
    
    template < class G >
    typename DFSurvey<G>::AdjIterator DFSurvey<G>::NextNeighbor (Vertex x)
    {
        while (neighbor_[x] != g_.End(x) && 'w' != color_[*neighbor_[x]])
            ++neighbor_[x]; //this is a list iterator
        return neighbor_[x];
    }
    
    
    //Developer helper methods
    
    template < class G >
    void DFSurvey<G>::ShowQueSetup (std::ostream& os) const
    {
        os << "\n  conStack\n"
        << "  ------->\n";
    }
    
    template < class G >
    void DFSurvey<G>::ShowQue (std::ostream& os) const
    {
        os << "  ";
        if (conQ_.Empty())
            os << "NULL";
        else
            conQ_.Display(os, ' ');
        os << '\n';
    }

} //end namespace fsu

#endif /* DFSURVEY_H */
