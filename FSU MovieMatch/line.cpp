/*
   implements the private method
 
   static MovieMatch::Line( std::istream& , fsu::Vector<Name>& );
*/

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
