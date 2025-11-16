#include "texture.hpp"

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
	// Define o tipo de textura
	type = texType;

	// Armazena largura, altura e número de canais de cor da imagem
	int widthImg, heightImg, numColCh;
	
	// Inverte verticalmente a imagem para parecer correta
	stbi_set_flip_vertically_on_load(true);
	
	// Lê a imagem de um arquivo e armazena em bytes
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	// Gera um objeto de textura OpenGL
	glGenTextures(1, &ID);
	
	// Atribui a textura a uma Unidade de Textura
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	// Configura o tipo de algoritmo para ampliar ou reduzir a imagem
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Configura como a textura se repete
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Linhas extras em caso de usar GL_CLAMP_TO_BORDER
	//float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

	// Atribui a imagem ao objeto de Textura OpenGL
	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
	
	// Gera MipMaps
	glGenerateMipmap(texType);

	// Deleta os dados da imagem pois já estão no objeto de Textura OpenGL
	stbi_image_free(bytes);

	// Desvincula o objeto de Textura OpenGL para evitar modificações acidentais
	glBindTexture(texType, 0);
}

void Texture::bindUnit(Shader& shader, const char* uniform, GLuint unit)
{
	// Obtém a localização do uniforme no shader
	GLuint texUni = glGetUniformLocation(shader.id, uniform);
	
	// O shader precisa estar ativo antes de alterar um uniforme
	shader.activate();
	
	// Define o valor do uniforme
	glUniform1i(texUni, unit);
}

void Texture::bind()
{
	glBindTexture(type, ID);
}

void Texture::unbind()
{
	glBindTexture(type, 0);
}

void Texture::deleteIt()
{
	glDeleteTextures(1, &ID);
}