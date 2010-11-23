using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Maptool
{
    class Map
    {
        private List<Entity> entities;

        public List<Entity> Entities
        {
            get { return entities; }
            set { entities = value; }
        }

        /**
         * Saves a map
         **/
        public bool Save(string filename)
        {
            string o = "";

            foreach (Entity ent in entities) {
                if (ent is WallEntity) {
                    o += "Wall {";
                    o += "  type = \"" + ((WallEntity)ent).Type.Name + "\"";
                }

                o += "  x = " + ent.X;
                o += "  y = " + ent.Y;
                o += "  angle = " + ent.Angle;
                o += "  }\n";
            }

            System.IO.File.WriteAllText(filename, o);

            return true;
        }
    }
}
