using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using Ionic.Utils.Zip;
using System.IO;

namespace datatool
{
    public class DataProvider
    {
        ConfuseReader read;
        private string datapath;
        private ZipFile zf;

        private List<base_item> modifiers;
        private List<base_item> unitclasses;
        private List<base_item> weapontypes;
        private List<base_item> animmodels;


        public DataProvider()
        {
            this.modifiers = new List<base_item>();
            this.unitclasses = new List<base_item>();
            this.weapontypes = new List<base_item>();
            this.animmodels = new List<base_item>();
        }


        /**
        * Loads data files
        **/
        public bool load(string filename, bool isZip)
        {
            this.datapath = filename;
            this.read = new ConfuseReader();

            this.modifiers.Clear();
            this.unitclasses.Clear();
            this.weapontypes.Clear();
            this.animmodels.Clear();

            if (isZip) {
                this.zf = new ZipFile(filename);
            } else {
                this.zf = null;
            }

            load_weapontypes();
            load_unittypes();
            load_animmodels();

            return true;
        }


        /**
         * The currently loaded datapath.
         **/
        public string Filename
        {
            get { return this.datapath; }
        }

        /**
         * Is it a ZIP currently loaded?
         **/
        public bool IsZip
        {
            get { return (this.zf != null); }
        }

        public string GetFriendlyFilename()
        {
            if (this.zf != null) {
                return "[Mod; " + Path.GetFileName(this.datapath) + "]";
            } else {
                return "[Directory; " + this.datapath + "]";
            }
        }

        public void SetFile(string filename, bool zip)
        {
            this.datapath = filename;
            if (zip) {
                this.zf = new ZipFile(filename);
            } else {
                this.zf = null;
            }
        }


        /**
         * Read a file from the disk or zip
         **/
        private string readFile(string filename)
        {
            if (this.zf != null) {
                MemoryStream ms = new MemoryStream();
                zf.Extract(filename, ms);
                ms.Seek(0, 0);
                StreamReader reader = new StreamReader(ms);
                return reader.ReadToEnd();

            } else {
                return File.ReadAllText(this.datapath + "\\" + filename);
            }
        }

        /**
         * Write a file to the disk or zip
         **/
        private void writeFile(string filename, string content)
        {
            if (this.zf != null) {
                try {
                    zf.RemoveEntry(filename);
                } catch (Exception ex) {}
                zf.AddStringAsFile(content, filename, "");

            } else {
                File.WriteAllText(this.datapath + "\\" + filename, content);
            }
        }
        


        /**
        * Saves data files
        **/
        public bool save()
        {
            string o;
            
            // Weapons
            o = "";
            foreach (base_item i in this.weapontypes) {
                o += "weapon {\n\t" + i.getConfItem() + "\n}\n";
            }
            this.writeFile("weapontypes.conf", o);

            if (this.zf != null) {
                this.zf.Save();
                this.zf = new ZipFile(this.datapath);
            }

            return true;
        }


        

        /**
        * Loads the particletypes
        **/
        private void load_weapontypes()
        {
            ConfuseSection sect = null;
            string file = this.readFile("weapontypes.conf");

            // Parse
            try {
                sect = read.Parse(file);
            } catch (Exception ex) {
                MessageBox.Show("Error loading weapontypes:\n" + ex.Message);
                return;
            }

            // Load
            foreach (ConfuseSection s in sect.subsections) {
                weapontype_item i = new weapontype_item("");
                weapontypes.Add(i);

                if (!s.values.ContainsKey("name")) throw new Exception("Weapontype defined without a name");

                i.fillData(s);

                i.Sounds.Clear();
                foreach (ConfuseSection ss in s.subsections) {
                    if (ss.name == "sound") {
                        weapontype_sound sss = new weapontype_sound();
                        sss.Sound = ss.get_string("sound", "");
                        sss.Type = ss.get_int("type", 0);
                        i.Sounds.Add(sss);
                    }
                }
            }
        }


        /**
        * Loads the particletypes
        **/
        private void load_unittypes()
        {
            ConfuseSection sect = null;
            string file = this.readFile("unittypes.conf");

            // Parse
            try {
                sect = read.Parse(file);
            } catch (Exception ex) {
                MessageBox.Show("Error loading unit types:\n" + ex.Message);
                return;
            }

            // Load
            foreach (ConfuseSection s in sect.subsections) {
                unitclass_item i = new unitclass_item();
                unitclasses.Add(i);

                if (!s.values.ContainsKey("name")) throw new Exception("Unit type defined without a name");

                i.fillData(s);

                foreach (ConfuseSection ss in s.subsections) {
                    if (ss.name == "sound") {
                        unitclass_sound subitem = new unitclass_sound();
                        subitem.fillData(ss);
                        i.Sounds.Add(subitem);

                    } else if (ss.name == "settings") {

                    } else if (ss.name == "state") {
                        unitclass_state subitem = new unitclass_state();
                        subitem.fillData(ss);
                        i.Models.Add(subitem);

                    }
                }
            }
        }


        /**
        * Loads the particletypes
        **/
        private void load_animmodels()
        {
            ConfuseSection sect = null;
            string file = this.readFile("animmodels.conf");
            read.file = "animmodels.conf";
            sect = read.Parse(file);

            // Load
            foreach (ConfuseSection s in sect.subsections) {
                animmodel_item i = new animmodel_item();
                animmodels.Add(i);

                if (!s.values.ContainsKey("name")) throw new Exception("Anim model defined without a name");

                i.fillData(s);
            }
        }



        public List<base_item> AnimModels
        {
            get { return animmodels; }
        }

        public List<base_item> Modifiers
        {
            get { return modifiers; }
        }

        public List<base_item> UnitClasses
        {
            get { return unitclasses; }
        }

        public List<base_item> WeaponTypes
        {
            get { return weapontypes; }
        }

    }
}
