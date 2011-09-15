using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    [Serializable]
    public class particletype_item : base_item
    {
        private string name;
        private string model;
        private range max_speed;
        private range begin_speed;
        private range accel;
        private range age;
        private range unit_damage;
        private range wall_damage;
        private range unit_hits;
        private range wall_hits;


        public particletype_item(string name)
        {
            this.name = name;
            this.model = "";
            this.max_speed = new range(0);
            this.begin_speed = new range(0);
            this.accel = new range(0);
            this.age = new range(0);
            this.unit_damage = new range(0);
            this.wall_damage = new range(0);
            this.unit_hits = new range(1);
            this.wall_hits = new range(1);
        }

        override public string getName()
        {
            return this.name;
        }



        [DescriptionAttribute("The name of this particletype, internal use only")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }

        [DescriptionAttribute("The animmodel to display for this particletype")]
        public string Model
        {
            get { return this.model; }
            set { this.model = value; }
        }

        [DescriptionAttribute("Max speed")]
        [Editor(typeof(range_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string MaxSpeed
        {
            get { return this.max_speed.toString(); }
            set { this.max_speed = range.fromString(value); }
        }

        [DescriptionAttribute("Initial speed")]
        [Editor(typeof(range_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string BeginSpeed
        {
            get { return this.begin_speed.toString(); }
            set { this.begin_speed = range.fromString(value); }
        }

        [DescriptionAttribute("Acceleration of the speed, in pixels/second")]
        [Editor(typeof(range_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string Accel
        {
            get { return this.accel.toString(); }
            set { this.accel = range.fromString(value); }
        }

        [DescriptionAttribute("The max age of the particle")]
        [Editor(typeof(range_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string Age
        {
            get { return this.age.toString(); }
            set { this.age = range.fromString(value); }
        }

        [DescriptionAttribute("Damage against units")]
        [Editor(typeof(range_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string UnitDamage
        {
            get { return this.unit_damage.toString(); }
            set { this.unit_damage = range.fromString(value); }
        }

        [DescriptionAttribute("Number of hits against units before this particle is removed")]
        [Editor(typeof(range_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string UnitHits
        {
            get { return this.unit_hits.toString(); }
            set { this.unit_hits = range.fromString(value); }
        }

        [DescriptionAttribute("Damage against walls and solid structures")]
        [Editor(typeof(range_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string WallDamage
        {
            get { return this.wall_damage.toString(); }
            set { this.wall_damage = range.fromString(value); }
        }

        [DescriptionAttribute("Number of hits against walls before this particle is removed")]
        [Editor(typeof(range_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string WallHits
        {
            get { return this.wall_hits.toString(); }
            set { this.wall_hits = range.fromString(value); }
        }
    }
}
