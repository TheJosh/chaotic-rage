using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Maptool
{
    class WallEntity : Entity
    {
        private string type;

        public string Type
        {
            get { return type; }
            set { type = value; }
        }
    }
}
