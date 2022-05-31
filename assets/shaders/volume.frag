#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT {
    vec3 FragPos;
    vec3 TexCoord;
} fs_in;

struct Light {
    vec3 position;
    vec3 color;
};

uniform sampler3D volume;
uniform sampler1D transfer_function;
uniform vec3 volume_resolution;
uniform vec3 volume_ratio;
uniform float sample_rate;
uniform vec3 background_color;

uniform float shininess;
uniform vec3 viewPos;
uniform bool useLighting;
uniform bool useNormalColor;

uniform float bloomThreshold;

uniform Light light;

vec3 BlinnPhongShading(vec3 normal, vec3 color, vec3 position) {
    // Ambient
    float ambient_strength = 0.2f;
    vec3 ambient = ambient_strength * light.color;

    // Diffuse
    float diffuse_strength = 0.75f;
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - position);
    float diff = dot(norm, lightDir);
    if (diff <= 0) {
        diff *= -1;
        norm = -norm;
    }
    vec3 diffuse = diffuse_strength * diff * light.color;

    // Specular
    float specular_strength = 0.4f;
    vec3 viewDir = normalize(viewPos - position);
    vec3 halfway = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfway), 0.0f), shininess);
    vec3 specular = specular_strength * spec * light.color;

    vec3 result = clamp(vec3(ambient + diffuse + specular) * color, 0.0f, 1.0f);
    return result;
}

void main () {
    vec4 result = vec4(background_color, 0.0f);
    vec3 ray_direction = normalize(fs_in.FragPos - viewPos);

    vec3 sample_pos = fs_in.TexCoord;
    vec3 current_pos = fs_in.FragPos;

    while (true) {
        // 透過 sample_pos 取樣 volume 的法向量以及 Volume Value (對應顏色)
        vec4 volume_data = texture(volume, sample_pos);
        vec4 volume_color = texture(transfer_function, volume_data.a);
        if (useNormalColor) {
            volume_color.r = volume_data.r;
            volume_color.g = volume_data.g;
            volume_color.b = volume_data.b;
        }

        // 取樣後立即更新位置(沿著視線)，而實際的位置也改變，相對材質座標也要改變
        current_pos = current_pos + ray_direction * sample_rate;
        vec3 ray_direction_in_texture = vec3(1.0f);
        vec3 actal_res = volume_resolution * volume_ratio;
        ray_direction_in_texture.x = ray_direction.x / (actal_res.x);
        ray_direction_in_texture.y = ray_direction.y / (actal_res.y);
        ray_direction_in_texture.z = ray_direction.z / (actal_res.z);
        sample_pos = sample_pos + ray_direction_in_texture * sample_rate;

        // 如果射線出界了請離開迴圈
        if (current_pos.x < -actal_res.x / 2.0f || current_pos.x > actal_res.x / 2.0f) {
            break;
        }
        if (current_pos.y < -actal_res.y / 2.0f || current_pos.y > actal_res.y / 2.0f) {
            break;
        }
        if (current_pos.z < -actal_res.z / 2.0f || current_pos.z > actal_res.z / 2.0f) {
            break;
        }

        // 如果發現該 voxel 透過 transfer function 得來的 alpha 值是 0，那就不用算光照直接看一個
        if (volume_color.a == 0) {
            continue;
        }

        // 計算光照
        vec3 temp_color = vec3(0.0f);
        if (useLighting) {
            temp_color = BlinnPhongShading(volume_data.xyz, volume_color.rgb, current_pos);
        } else {
            temp_color = volume_color.rgb * 2.0f;
        }

        result.rgb += (1.0f - result.a) * volume_color.a * temp_color;
        result.a += (1.0f - result.a) * volume_color.a;

        // 如果累積濃霧已經超過 0.99，就也不用再算後面的顏色了
        if (result.a > 0.99f) {
            break;
        }
    }

    // vec4 bg_color = vec4(background_color, 1.0f);
    vec4 final_frag_color = result;
    // final_frag_color.a = result.a + bg_color.a - result.a * bg_color.a;
    // final_frag_color.rgb = bg_color.a * bg_color.rgb * (1.0f - result.a) + result.rgb * result.a;

    // 計算亮度有沒有超過 1.0f，用於泛光特效使用
    vec4 bright_color = vec4(0.0f);
    float brightness = dot(final_frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > bloomThreshold) {
        bright_color = vec4(final_frag_color.rgb, 1.0f);
    } else {
        bright_color = vec4(0, 0, 0, 1.0f);
    }

    BrightColor = vec4(bright_color.rgb, 1.0f);
    FragColor = final_frag_color;
}