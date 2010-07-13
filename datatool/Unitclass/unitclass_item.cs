using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    public class unitclass_settings
    {
        private string name;
        private int lin_speed;
        private int lin_accel;
        private int turn_move;
        private int turn_aim;

        public unitclass_settings clone()
        {
            unitclass_settings ret;
            ret = new unitclass_settings();
            ret.name = this.name;
            ret.lin_speed = this.lin_speed;
            ret.lin_accel = this.lin_accel;
            ret.turn_aim = this.turn_aim;
            ret.turn_move = this.turn_move;
            return ret;
        }

        [DescriptionAttribute("Internal use only")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }

        [DescriptionAttribute("The maximum speed attainable, pixels/second")]
        public int LinearSpeed
        {
            get { return this.lin_speed; }
            set { this.lin_speed = value; }
        }

        [DescriptionAttribute("The rate of acceleration, pixels/second/second")]
        public int LinearAcceleration
        {
            get { return this.lin_accel; }
            set { this.lin_accel = value; }
        }

        [DescriptionAttribute("The speed of player turns")]
        public int MoveTurnSpeed
        {
            get { return this.turn_move; }
            set { this.turn_move = value; }
        }

        [DescriptionAttribute("The speed of aiming turns")]
        public int AimTurnSpeed
        {
            get { return this.turn_aim; }
            set { this.turn_aim = value; }
        }

    }

    public class unitclass_item : actions_item 
    {
        private string name;
        private List<unitclass_settings> settings;


        public unitclass_item(string name)
        {
            this.name = name;
            this.settings = new List<unitclass_settings>();

            this.settings.Add(new unitclass_settings());
        }

        override public string getName()
        {
            return this.name;
        }

        public unitclass_item clone()
        {
            unitclass_item ret;
            ret = new unitclass_item(this.name);

            ret.settings.Clear();
            for (int i = 0; i < this.settings.Count; i++) {
                ret.settings.Add(this.settings[i].clone());
            }

            return ret;
        }



        [DescriptionAttribute("The name of this unitclass, internal use only")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }

        [DescriptionAttribute("The available settings")]
        public List<unitclass_settings> Settings
        {
            get { return this.settings; }
        }

    }
}
