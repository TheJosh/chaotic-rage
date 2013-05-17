using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace Maptool
{
    abstract public class Entity
    {
        private int x;
        private int y;
        private int angle;
        private EntityType type;



        [Category("General")]
        [Description("The type of entity")]
        public EntityType Type
        {
            get { return type; }
            set { type = value; }
        }

        [Category("General")]
        [Description("The X co-ordinate of the entity")]
        public int X
        {
            get { return x; }
            set { x = value; }
        }

        [Category("General")]
        [Description("The Y co-ordinate of the entity")]
        public int Y
        {
            get { return y; }
            set { y = value; }
        }

        [Category("General")]
        [Description("The angle of the entity, in degrees")]
        public int Angle
        {
            get { return angle; }
            set { angle = value; }
        }
    }
}
