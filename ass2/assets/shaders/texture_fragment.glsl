        #version 330 core

        in vec3 vertexColor;
        in vec2 vertexUV;
        
        uniform sampler2D textureSampler;
        uniform mat4 textureMatrix = mat4(1.0f);
        
        out vec4 FragColor;
        void main()
        {
           vec4 textureColor = texture(textureSampler, vertexUV );
           FragColor = textureColor * vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);
        }