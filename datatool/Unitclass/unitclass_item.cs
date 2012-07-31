using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace datatool
{
    [Serializable]
    [DisplayName("Unit settings group")]
    public class unitclass_settings : base_item
    {
        private string name;
        private modifier_item modifier;
        private int lin_speed;
        private int lin_accel;
        private int turn_move;
        private int turn_aim;

        [DescriptionAttribute("Internal use only")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }

        [DescriptionAttribute("The modifier this settings group applies for")]
        [Editor(typeof(modifier_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public modifier_item Modifier
        {
            get { return this.modifier; }
            set { this.modifier = value; }
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
        
        override public string getName() { return this.name; }
    }


    public enum unitclass_state_types
    {
        WALK = 0,
        STATIC = 1,
        RUNNING = 2,
        FIRING = 3,
    }

    [Serializable]
    public class unitclass_state : base_item
    {
        private string name;
        private unitclass_state_types type;
        private string image;

        public unitclass_state() { }
        public unitclass_state(unitclass_state_types type) { this.name = type.ToString().ToLower();  this.type = type; }

        [DescriptionAttribute("Internal use only")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }

        [DescriptionAttribute("The unit state to use this animation for. If multiple animations are specified for a specific type, one is chosen at random")]
        public unitclass_state_types Type
        {
            get { return this.type; }
            set { this.type = value; }
        }

        [DescriptionAttribute("The name of the image to use for this animation")]
        [Editor(typeof(image_ui_editor), typeof(System.Drawing.Design.UITypeEditor))]
        public string Image
        {
            get { return this.image; }
            set { this.image = value; }
        }

        override public string getName() { return this.name; }
    }

    [Serializable]
    public class unitclass_sound : base_item
    {
        private string sound;
        private int type;
        
        public unitclass_sound() { }
        
        [DescriptionAttribute("Sound to play")]
        public string Sound
        {
            get { return this.sound; }
            set { this.sound = value; }
        }

        [DescriptionAttribute("What the sound is played for")]
        public int Type
        {
            get { return this.type; }
            set { this.type = value; }
        }

        override public string getName() { return this.sound; }
    }

    [Serializable]
    public class unitclass_item : base_item 
    {
        private string name;

        private List<unitclass_settings> settings;
        private List<unitclass_state> states;
        private List<unitclass_sound> sounds;
        private bool playable;
        private int begin_health;
        private int melee_damage;
        private int melee_delay;
        private int melee_cooldown;
        

        public unitclass_item(string name)
        {
            this.name = name;
            this.begin_health = 0;
            this.melee_damage = 100;
            this.melee_delay = 100;
            this.melee_cooldown = 100;

            this.settings = new List<unitclass_settings>();
            this.settings.Add(new unitclass_settings());

            this.states = new List<unitclass_state>();
            this.states.Add(new unitclass_state());

            this.sounds = new List<unitclass_sound>();
            this.sounds.Add(new unitclass_sound());
        }

        override public string getName()
        {
            return this.name;
        }



        [DescriptionAttribute("The name of this unitclass, internal use only")]
        [Category("General")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }

        [Category("General")]
        public bool Playable
        {
            get { return this.playable; }
            set { this.playable = value; }
        }

        [DescriptionAttribute("The initial health of the unit")]
        [Category("Balance Settings")]
        public int BeginHealth
        {
            get { return this.begin_health; }
            set { this.begin_health = value; }
        }

        [DescriptionAttribute("The amount of damage a melee attack does")]
        [Category("Balance Settings")]
        public int MeleeDamage
        {
            get { return this.melee_damage; }
            set { this.melee_damage = value; }
        }

        [DescriptionAttribute("The delay between initalising the melee and the damage is applied")]
        [Category("Balance Settings")]
        public int MeleeDelay
        {
            get { return this.melee_delay; }
            set { this.melee_delay = value; }
        }

        [DescriptionAttribute("The delay after a melee before another melee can be made")]
        [Category("Balance Settings")]
        public int MeleeCooldown
        {
            get { return this.melee_cooldown; }
            set { this.melee_cooldown = value; }
        }

        [Category("Balance Settings")]
        public List<unitclass_settings> Settings
        {
            get { return this.settings; }
        }

        [Category("Audio/Video")]
        public List<unitclass_state> States
        {
            get { return this.states; }
        }

        [Category("Audio/Video")]
        public List<unitclass_sound> Sounds
        {
            get { return this.sounds; }
        }

    }
}
