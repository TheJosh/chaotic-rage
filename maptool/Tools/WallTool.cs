using System;
using System.Collections.Generic;
using System.Text;
using datatool;

namespace Maptool
{
    class WallTool : Tool
    {
        List<EntityType> types;

        public WallTool() {
            types = new List<EntityType>();

            ConfuseReader rdr = new ConfuseReader();
            String file = System.IO.File.ReadAllText(Program.Datapath + "/walltypes/walltypes.conf");
            ConfuseSection sect = rdr.Parse(file);

            foreach (ConfuseSection walltype in sect.subsections) {
                if (walltype.name != "walltype") continue;

                WallEntityType et = new WallEntityType(walltype.get_string("name", ""));
                if (et.Name == "") continue;

                types.Add(et);
            }
        }

        public override Entity createEntity()
        {
            return new WallEntity();
        }

        public override bool isThisOurs(Entity ent)
        {
            return (ent is WallEntity);
        }

        public override bool isAreaBased()
        {
            return false;
        }

        public override List<EntityType> getTypes()
        {
            return types;
        }
    }
}
