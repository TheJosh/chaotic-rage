using System;
using System.Collections.Generic;
using System.Text;
using datatool;

namespace Maptool
{
    class ObjectTool : Tool
    {
        private List<EntityType> types;

        public ObjectTool()
        {
            types = new List<EntityType>();


            ConfuseReader rdr = new ConfuseReader();
            String file = System.IO.File.ReadAllText(Program.Datapath + "/objecttypes.conf");
            ConfuseSection sect = rdr.Parse(file);

            foreach (ConfuseSection walltype in sect.subsections) {
                if (walltype.name != "objecttype") continue;

                ObjectEntityType et = new ObjectEntityType(walltype.get_string("name", ""));
                if (et.Name == "") continue;

                types.Add(et);
            } 
        }


        public override Entity createEntity()
        {
            return new ObjectEntity();
        }

        public override bool isThisOurs(Entity ent)
        {
            return (ent is ObjectEntity);
        }

        public override bool isAreaBased()
        {
            return true;
        }

        public override List<EntityType> getTypes()
        {
            return types;
        }

        public override string saveConfig(Entity ent)
        {
            string o = "";
            ObjectEntity z = (ObjectEntity)ent;

            o += "object {";
            o += "  type = \"" + z.Type.Name + '"';
            o += "  x = " + z.X;
            o += "  y = " + z.Y;
            o += "}";

            return o;
        }

        public override Entity loadConfig(ConfuseSection sect)
        {
            ObjectEntity ent = new ObjectEntity();

            // find type
            string type = sect.get_string("type", "");
            foreach (EntityType ty in this.types) {
                if (ty.Name == type) {
                    ent.Type = ty;
                    break;
                }
            }
            if (ent.Type == null) return null;

            // load other deets
            ent.X = sect.get_int("x", 0);
            ent.Y = sect.get_int("y", 0);

            return ent;
        }
    }
}
