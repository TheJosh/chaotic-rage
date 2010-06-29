using System;
using System.Collections.Generic;
using System.Text;

namespace datatool
{
    public abstract class base_item
    {
        abstract public string getName();

        override public string ToString()
        {
            return this.getName();
        }
    }
}
