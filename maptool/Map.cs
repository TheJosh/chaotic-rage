using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using datatool;

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
                    o += "wall {";
                    o += "  type = \"" + ((WallEntity)ent).Type.Name + "\"";

                } else if (ent is ZoneEntity) {
                    o += "zone {";
                    o += "  width = " + ((ZoneEntity)ent).Width;
                    o += "  height = " + ((ZoneEntity)ent).Height;
                }

                o += "  x = " + ent.X;
                o += "  y = " + ent.Y;
                o += "  angle = " + ent.Angle;
                o += "  }\n";
            }

            System.IO.File.WriteAllText(filename, o);

            return true;
        }


        /**
         * Load
         **/
        public bool Load(string filename)
        {
            ConfuseReader rdr = new ConfuseReader();
            string input = System.IO.File.ReadAllText(filename);
            ConfuseSection parsed = rdr.Parse(input);

            List<EntityType> wall_types = (new WallTool()).getTypes();
            List<EntityType> zone_types = (new ZoneTool()).getTypes();

            entities = new List<Entity> ();

            foreach (ConfuseSection sect in parsed.subsections) {
                Entity ent = null;

                if (sect.name == "wall") {
                    ent = new WallEntity();

                    string type = sect.get_string("type", "");
                    ent.Type = wall_types.Find(delegate(EntityType ty) { return ty.Name == type; });

                } else if (sect.name == "zone") {
                    ent = new ZoneEntity();
                    ent.Type = zone_types[0];
                    ((ZoneEntity)ent).Width = sect.get_int("width", 20);
                    ((ZoneEntity)ent).Height = sect.get_int("height", 20);
                }

                if (ent == null) continue;
                if (ent.Type == null) continue;

                ent.X = sect.get_int("x", 0);
                ent.Y = sect.get_int("y", 0);
                ent.Angle = sect.get_int("angle", 0);

                entities.Add(ent);
            }
            
            return true;
        }
    }
}
