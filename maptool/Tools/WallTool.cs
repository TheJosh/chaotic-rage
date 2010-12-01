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
            String file = System.IO.File.ReadAllText(Program.Datapath + "/animmodels/animmodels.conf");
            ConfuseSection sect = rdr.Parse(file);

            string texture;
            Dictionary<string, string> animtex = new Dictionary<string, string>();
            foreach (ConfuseSection animmodel in sect.subsections) {
                if (animmodel.name != "animmodel") continue;

                foreach (ConfuseSection meshframe in animmodel.subsections) {
                    if (meshframe.name != "meshframe") continue;

                    texture = meshframe.get_string("texture", "");
                    if (texture != "") {
                        animtex.Add(animmodel.get_string("name", ""), texture);
                        break;
                    }
                }
            }


            rdr = new ConfuseReader();
            file = System.IO.File.ReadAllText(Program.Datapath + "/walltypes/walltypes.conf");
            sect = rdr.Parse(file);

            foreach (ConfuseSection walltype in sect.subsections) {
                if (walltype.name != "walltype") continue;

                WallEntityType et = new WallEntityType(walltype.get_string("name", ""));
                if (et.Name == "") continue;

                types.Add(et);

                if (animtex.ContainsKey(walltype.get_string("model", ""))) {
                    et.Image = new System.Drawing.Bitmap(Program.Datapath + "/animmodels/" + animtex[walltype.get_string("model", "")] + ".png");
                }
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
