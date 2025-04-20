using System.ComponentModel;
using Reloaded.Mod.Interfaces.Structs;
using Sonic_Heroes_MouseCam_Config.Template.Configuration;

namespace Sonic_Heroes_MouseCam_Config.Configuration
{
    public class Config : Configurable<Config>
    {
        /*
            User Properties:
                - Please put all of your configurable properties here.
    
            By default, configuration saves as "Config.json" in mod user config folder.    
            Need more config files/classes? See Configuration.cs
    
            Available Attributes:
            - Category
            - DisplayName
            - Description
            - DefaultValue

            // Technically Supported but not Useful
            - Browsable
            - Localizable

            The `DefaultValue` attribute is used as part of the `Reset` button in Reloaded-Launcher.
        */

        [DisplayName("Sensivity")]
        [Description("Sensivity 0.1 - 1000.0")]
        [DefaultValue(10.0f)]
        public float Sensivity { get; set; } = 10.0f;
    }

    /// <summary>
    /// Allows you to override certain aspects of the configuration creation process (e.g. create multiple configurations).
    /// Override elements in <see cref="ConfiguratorMixinBase"/> for finer control.
    /// </summary>
    public class ConfiguratorMixin : ConfiguratorMixinBase
    {
        // 
    }
}
