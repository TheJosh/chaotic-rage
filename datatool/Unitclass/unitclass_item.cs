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
        DIE = 4,
        MELEE = 5,
    }


    [Serializable]
    public class unitclass_state : base_item
    {
        private unitclass_state_types type;
        private string model;

        public unitclass_state() { }
        public unitclass_state(unitclass_state_types type) {
            this.type = type;
        }

        [DescriptionAttribute("The unit state to use this animation for. If multiple animations are specified for a specific type, one is chosen at random")]
        public unitclass_state_types Type
        {
            get { return this.type; }
            set { this.type = value; }
        }

        [ConfuseInt("type")]
        [Browsable(false)]
        public int TypeInt { set { this.type = (unitclass_state_types)value; } get { return (int) this.type; } }


        [DescriptionAttribute("The model to show for this unit state")]
        [ConfuseString("model")]
        public string Model
        {
            get { return this.model; }
            set { this.model = value; }
        }

        override public string getName() { return this.type.ToString(); }
    }


    public enum unitclass_sound_types
    {
        STATIC = 1,
        HIT = 2,
        DEATH = 3,
        SPAWN = 4,
        ABILITy = 5,
        CELEBRATE = 6,
        FAIL = 7
    }

    [Serializable]
    public class unitclass_sound : base_item
    {
        private string sound;
        private unitclass_sound_types type;
        
        public unitclass_sound() { }
        
        [DescriptionAttribute("Sound to play")]
        [ConfuseString("sound")]
        public string Sound
        {
            get { return this.sound; }
            set { this.sound = value; }
        }

        [DescriptionAttribute("What the sound is played for")]
        public unitclass_sound_types Type
        {
            get { return this.type; }
            set { this.type = value; }
        }

        [ConfuseInt("type")]
        [Browsable(false)]
        public int TypeInt
        {
            get { return (int) this.type; }
            set { this.type = (unitclass_sound_types) value; }
        }

        override public string getName() { return this.type.ToString(); }
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


        public unitclass_item()
        {
            this.settings = new List<unitclass_settings>();
            this.states = new List<unitclass_state>();
            this.sounds = new List<unitclass_sound>();
        }

        override public string getName()
        {
            return this.name;
        }



        [DescriptionAttribute("The name of this unitclass, internal use only")]
        [Category("General")]
        [ConfuseString("name")]
        public string Name
        {
            get { return this.name; }
            set { this.name = value; }
        }

        [Category("General")]
        [ConfuseBoolean("playable", true)]
        public bool Playable
        {
            get { return this.playable; }
            set { this.playable = value; }
        }

        [DescriptionAttribute("The initial health of the unit")]
        [Category("Gameplay")]
        [ConfuseInt("begin_health", 0)]
        public int BeginHealth
        {
            get { return this.begin_health; }
            set { this.begin_health = value; }
        }

        [DescriptionAttribute("The amount of damage a melee attack does")]
        [Category("Gameplay")]
        [ConfuseInt("melee_damage", 100)]
        public int MeleeDamage
        {
            get { return this.melee_damage; }
            set { this.melee_damage = value; }
        }

        [DescriptionAttribute("The delay between initalising the melee and the damage is applied")]
        [Category("Gameplay")]
        [ConfuseInt("melee_delay", 100)]
        public int MeleeDelay
        {
            get { return this.melee_delay; }
            set { this.melee_delay = value; }
        }

        [DescriptionAttribute("The delay after a melee before another melee can be made")]
        [Category("Gameplay")]
        [ConfuseInt("melee_cooldown", 100)]
        public int MeleeCooldown
        {
            get { return this.melee_cooldown; }
            set { this.melee_cooldown = value; }
        }

        [Category("Gameplay")]
        public List<unitclass_settings> Settings
        {
            get { return this.settings; }
        }

        [Category("Audio/Video")]
        public List<unitclass_state> Models
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
