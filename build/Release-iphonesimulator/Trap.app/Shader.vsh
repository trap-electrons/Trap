//
//  Shader.vsh
//  Trap
//
//  Created by Max Tkachenko on 10/5/10.
//  Copyright Maxim Tkachenko 2010. All rights reserved.
//

attribute vec4 position;
attribute vec4 color;

varying vec4 colorVarying;

uniform float translate;

void main()
{
    gl_Position = position;
    gl_Position.y += sin(translate) / 2.0;

    colorVarying = color;
}
