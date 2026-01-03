using System.ComponentModel;
using System.ComponentModel.DataAnnotations;
using Mouse_Camera_Config.Template.Configuration;
using Reloaded.Mod.Interfaces.Structs;

namespace Mouse_Camera_Config.Configuration
{
    public class Config : Configurable<Config>
    {
        [DisplayName("Sensivity X")]
        [Description("Sensivity for Yaw value")]
        [DefaultValue(10.0f)]
        public float SensivityX { get; set; } = 10.0f;

        [DisplayName("Sensivity Y")]
        [Description("Sensivity for Pitch value")]
        [DefaultValue(10.0f)]
        public float SensivityY { get; set; } = 10.0f;
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
