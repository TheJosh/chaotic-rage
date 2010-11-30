using System;
using System.Collections.Generic;
using System.Text;

namespace Maptool
{
    class ZoneEntity : Entity
    {
        private int width;
        private int height;

        public ZoneEntity()
        {
            this.width = 20;
            this.height = 20;
        }

        public int Width
        {
            get { return width; }
            set { width = value; }
        }

        public int Height
        {
            get { return height; }
            set { height = value; }
        }
    }
}
