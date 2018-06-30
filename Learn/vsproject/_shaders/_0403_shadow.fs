#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform sampler2D depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    //执行透视除法 => NDC坐标(x,y,z ∈ [-1,1])
    //注：当使用正交投影矩阵，顶点w元素仍保持不变，所以这一步实际上毫无意义。
    //可是，当使用透视投影的时候就是必须的了，所以为了保证在两种投影矩阵下都有效就得留着这行。
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    //将NDC坐标限制在[0,1]; xy 则表示采样2d采样坐标，z分量则表示来自光的透视视角的片元的深度
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    float closestDepth = texture(depthMap, projCoords.xy).r; //表示在光视角下的最近的片源深度

    //根据表面朝向光线的角度更改偏移量：使用点乘：
    float bias =  max(0.00005 * (1.0 - dot(normal, lightDir)), 0.00001);
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    //percentage-closer filtering
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    if (currentDepth > 0.999)
        shadow = 0.0;
    return shadow;
}

void main()
{
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    vec3 colorSpec = texture(texture_specular1, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(.2);
    // Ambient
    vec3 ambient = 0.15 * color;
    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 1.0);
    vec3 diffuse = diff * lightColor * color;
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 1.0), 64.0);
    vec3 specular = spec * lightColor * color;
    // 计算阴影
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);

    FragColor = vec4(lighting, 1.0f);
}