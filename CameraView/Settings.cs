using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace CameraView
{
    class Settings
    {
        private static Settings instance;

        public string RecordSavePath = "Video";

        private Settings() { }

        public void Save()
        {
            using (StreamWriter sw = new StreamWriter("settings.xml"))
            {
                XmlSerializer xmls = new XmlSerializer(typeof(Settings));
                xmls.Serialize(sw, this);
            }
        }

        public static Settings GetInstance()
        {
            if (null == instance) {
                try {
                    using (StreamReader sw = new StreamReader("settings.xml")) {
                        XmlSerializer xmls = new XmlSerializer(typeof(Settings));
                        instance = (xmls.Deserialize(sw) as Settings);
                    }
                } catch (Exception) {
                    instance = new Settings();
                }
            }

            return instance;
        }
    }
}
