
# 🎨 Pixel Art Maker

Uma aplicação web interativa e simples para criar artes em pixel (Pixel Art). Este projeto permite que o usuário defina o tamanho da grade, escolha cores, desenhe livremente e salve suas criações como imagem PNG.

## 📖 Sobre o Projeto

O **Pixel Art Maker** é um projeto de Front-end desenvolvido para praticar manipulação do DOM, eventos de mouse e lógica de CSS Grid. O objetivo é fornecer uma tela em branco quadriculada onde o usuário pode pintar pixels individuais ou arrastar o mouse para desenhar continuamente.

## 🚀 Funcionalidades

- **Grade Dinâmica:** O usuário pode definir a altura e largura da grade (Inputs numéricos).
- **Seletor de Cores:** Input nativo (`type="color"`) para escolher qualquer cor hexadecimal.
- **Desenho Contínuo:** Funcionalidade de "clicar e arrastar" para pintar múltiplos pixels rapidamente.
- **Ferramenta Borracha:** Permite apagar pixels específicos (tornando-os transparentes).
- **Limpar Tela:** Botão para resetar todo o desenho sem recriar a grade.
- **Exportar Imagem:** Funcionalidade para baixar a arte criada em formato **.PNG** com fundo transparente, removendo as linhas da grade automaticamente na hora do salvamento.

## 🛠 Tecnologias Utilizadas

- **HTML5:** Estrutura semântica da página.
- **CSS3:** Estilização com Flexbox e CSS Grid para a grade de pixels.
- **JavaScript (Vanilla):** Lógica de manipulação do DOM e eventos.
- **[html2canvas](https://html2canvas.hertzen.com/):** Biblioteca externa utilizada para capturar a `div` da grade e convertê-la em uma imagem para download.
- **Material Icons:** Ícones utilizados nos botões para melhor experiência do usuário.

## 📂 Estrutura de Arquivos

```text
/
├── css/
│   └── estilos.css       # Estilos da interface e da grade
└── js/
    └── configuracoes.js  # Lógica do desenho, criação da grade e exportação
├── index.html            # Estrutura principal da página
```
## ⚡ Como Executar

Este projeto é uma aplicação web estática e não requer a instalação de ambientes complexos (como Node.js) para funcionar.

1. **Baixe o projeto** para o seu computador.
2. Certifique-se de manter a estrutura de pastas original (`index.html` na raiz, pastas `css` e `js`).
3. Dê um clique duplo no arquivo `index.html` para abri-lo em seu navegador padrão.

> **Recomendação:** Para garantir o funcionamento perfeito da biblioteca de exportação de imagem (`html2canvas`), sugere-se rodar o projeto através de um servidor local simples, como a extensão **Live Server** do VS Code, embora o arquivo funcione diretamente na maioria dos navegadores modernos.

## 🎮 Como Usar

1. **Configurar:** Defina a **Altura** e **Largura** desejadas para o seu quadro (valores sugeridos entre 10 e 50).
2. **Iniciar:** Clique no botão **Criar Grade** para gerar a área de desenho.
3. **Pintar:**
   - Escolha uma cor clicando no seletor de cores.
   - Clique em um quadrado para pintar um pixel.
   - **Arraste o mouse** enquanto segura o clique para pintar continuamente.
4. **Editar:**
   - Clique em **Borracha** para apagar pixels específicos.
   - Clique em **Limpar** para remover todo o desenho e recomeçar.
5. **Exportar:** Quando terminar, clique em **Salvar**. O desenho será baixado automaticamente como uma imagem `.png` com fundo transparente (as linhas da grade são removidas visualmente no arquivo final).

## 🤖 Pair Programming com IA

Este projeto foi desenvolvido utilizando a metodologia de *Pair Programming* com Inteligência Artificial. 

A IA foi **inteiramente responsável por toda a parte visual do projeto**, encarregando-se de:

- Criação da identidade visual e paleta de cores (tema *Dark Mode* com acentos coloridos).
- Desenvolvimento de todo o código CSS.
- Layout e diagramação dos elementos utilizando Flexbox e CSS Grid.
- Estilização dos componentes interativos (botões com efeitos de clique, inputs e ícones).
