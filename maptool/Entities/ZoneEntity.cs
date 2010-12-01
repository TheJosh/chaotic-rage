using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace Maptool
{
    class ZoneEntity : Entity
    {
        private int width;
        private int height;
        private ZoneFactions spawn;
        private ZoneFactions prison;
        private ZoneFactions collect;
        private ZoneFactions dest;
        private ZoneFactions nearbase;


        public ZoneEntity()
        {
            this.width = 20;
            this.height = 20;
            this.spawn = new ZoneFactions();
            this.prison = new ZoneFactions();
            this.collect = new ZoneFactions();
            this.dest = new ZoneFactions();
            this.nearbase = new ZoneFactions();
        }

        [Category("Size")]
        [Description("The width of this zone")]
        public int Width
        {
            get { return width; }
            set { width = value; }
        }

        [Category("Size")]
        [Description("The height of this zone")]
        public int Height
        {
            get { return height; }
            set { height = value; }
        }

        [Category("Purpose")]
        [Description("Spawn point for units")]
        [Editor(typeof(ZoneFactionsUI), typeof(System.Drawing.Design.UITypeEditor))]
        public ZoneFactions Spawn
        {
            get { return spawn; }
            set { spawn = value; }
        }

        [Category("Purpose")]
        [Description("Prison that captured units get placed into")]
        [Editor(typeof(ZoneFactionsUI), typeof(System.Drawing.Design.UITypeEditor))]
        public ZoneFactions Prison
        {
            get { return prison; }
            set { prison = value; }
        }

        [Category("Purpose")]
        [Description("Items which need to be picked up (i.e. flags)")]
        [Editor(typeof(ZoneFactionsUI), typeof(System.Drawing.Design.UITypeEditor))]
        public ZoneFactions Collection
        {
            get { return collect; }
            set { collect = value; }
        }

        [Category("Purpose")]
        [Description("Delivery location of picked-up items")]
        [Editor(typeof(ZoneFactionsUI), typeof(System.Drawing.Design.UITypeEditor))]
        public ZoneFactions Destination
        {
            get { return dest; }
            set { dest = value; }
        }

        [Category("Purpose")]
        [Description("A general-purpose location near the base but not in it")]
        [Editor(typeof(ZoneFactionsUI), typeof(System.Drawing.Design.UITypeEditor))]
        public ZoneFactions NearBase
        {
            get { return nearbase; }
            set { nearbase = value; }
        }
    }


    public class ZoneFactions
    {
        private bool [] factions = new bool[10];

        private enum faction_names
        {
            FACTION_INDIVIDUAL = 0,
            FACTION_TEAM1 = 1,
            FACTION_TEAM2 = 2,
            FACTION_TEAM3 = 3,
            FACTION_TEAM4 = 4,
            FACTION_TEAM5 = 5,
            FACTION_TEAM6 = 6,
            FACTION_TEAM7 = 7,
            FACTION_TEAM8 = 8,
            FACTION_COMMON = 9,
        };


        public override string ToString()
        {
            string o = "";

            for (int i = 0; i < 10; i++) {
                if (factions[i]) {
                    o += ((faction_names) i).ToString() + ", ";
                }
            }

            char [] trimchars = {' ', ','};
            return o.Trim(trimchars);
        }
    }

    public class ZoneFactionsUI : System.Drawing.Design.UITypeEditor
    {
        public override System.Drawing.Design.UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return System.Drawing.Design.UITypeEditorEditStyle.Modal;
        }

        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            /*range_ui_form form = new range_ui_form();

            form.value = range.fromString((string)value);

            if (form.ShowDialog() == System.Windows.Forms.DialogResult.OK) {
                return form.value.toString();
            }*/

            return value;
        }
    }
}
