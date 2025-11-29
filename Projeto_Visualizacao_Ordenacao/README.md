# 📊 Visualizador de Ordenação (Bubble Sort)

Este projeto é uma ferramenta interativa desenvolvida para visualizar o funcionamento do algoritmo de ordenação Bubble Sort em tempo real. Através de barras coloridas e animações, o usuário pode compreender a lógica de comparação e troca de elementos que ocorre durante o processo de ordenação.

---

#🚀 Funcionalidades

Input Personalizado: O usuário pode inserir qualquer sequência de números separados por vírgula.

Visualização Gráfica: Os números são convertidos em barras verticais com alturas proporcionais.

Animação Passo a Passo:

🔴 Vermelho: Indica os elementos que estão sendo comparados no momento.

🟣 Roxo: Estado padrão das barras não ordenadas.

🟢 Verde: Indica que o elemento já está na sua posição final correta.

Validação: Impede a execução se a entrada não for numérica.

---

# 🛠️ Stack Tecnológico
Este projeto foi construído utilizando tecnologias web nativas, sem dependência de frameworks externos:

![alt text](https://img.shields.io/badge/html5-%23E34F26.svg?style=for-the-badge&logo=html5&logoColor=white)
![alt text](https://img.shields.io/badge/css3-%231572B6.svg?style=for-the-badge&logo=css3&logoColor=white)
![alt text](https://img.shields.io/badge/javascript-%23323330.svg?style=for-the-badge&logo=javascript&logoColor=%23F7DF1E)
![alt text](https://img.shields.io/badge/Lógica_Algorítmica-5C2D91?style=for-the-badge)

---

# 🧠 Como funciona o Bubble Sort

O Bubble Sort (ou Ordenação em Bolha) é um dos algoritmos de ordenação mais simples, ideal para fins didáticos.

O Conceito

O nome "Bubble" vem da ideia de que os elementos maiores "flutuam" (como bolhas) para o topo (final da lista) a cada passagem completa pelo vetor.

O Processo no Algoritmo

Iteração: O algoritmo percorre a lista do início ao fim várias vezes.

Comparação: Em cada passo, ele compara dois elementos adjacentes (vizinhos).

Troca (Swap): Se o elemento da esquerda for maior que o elemento da direita, eles trocam de lugar.

Repetição: Esse processo se repete até que nenhuma troca seja necessária ou até que todos os elementos tenham sido fixados no final da lista.

# Complexidade
# * Melhor caso: O(n)O( _n_ ) (quando a lista já está ordenada).
# * Pior caso: O(n2)O( _n²_)(quando a lista está inversamente ordenada).

Neste visualizador: Você verá o par de barras ficar vermelho. Se a barra da esquerda for maior, elas trocam de posição visualmente. Ao final de cada passagem, a maior barra do grupo restante fica verde, indicando que está "travada" na posição correta.

---

# 📂 Como Executar o Projeto

Como este projeto é composto por um único arquivo HTML contendo o CSS e o JS, a execução é extremamente simples:

Baixe o código: Salve o código fornecido em um arquivo chamado index.html.

Abra no Navegador: Dê um clique duplo no arquivo ou arraste-o para qualquer navegador moderno (Chrome, Firefox, Edge, Safari).

Use a Ferramenta:

Digite números no campo de texto (ex: 50, 10, 80, 25).

Clique em "1. Carregar Barras".

Clique em "2. Ordenar!" e assista à mágica acontecer.

---
# 💻 Trecho de Código em Destaque

Abaixo está a função JavaScript responsável pela realização da ordenação, meu maior aprendizado durante o desenvolvimento deste projeto dentro da área que mais gosto, a lógica:

```code
  function gerarBarras() {
            const input = document.getElementById('inputNumeros').value;
            const container = document.getElementById('container-barras');
            const statusMsg = document.getElementById('statusMsg');
            
            container.innerHTML = ''; 
            statusMsg.innerText = ''; 

            const numeros = input.split(',').map(num => parseInt(num.trim()));

            if (numeros.some(isNaN)) {
                alert("Por favor, digite apenas números separados por vírgula.");
                return;
            }

            numeros.forEach(num => {
                const barra = document.createElement('div');
                barra.classList.add('barra');
                
                let alturaVisual = num * 3; 
                if(alturaVisual > 380) alturaVisual = 380;
                
                barra.style.height = `${alturaVisual}px`;
                barra.innerText = num;
                barra.setAttribute('data-valor', num);

                container.appendChild(barra);
            });
        }

        async function iniciarOrdenacao() {
            const container = document.getElementById('container-barras');
            let barras = document.getElementsByClassName('barra');
            const btnOrdenar = document.getElementById('btnOrdenar');
            const statusMsg = document.getElementById('statusMsg');
            
            btnOrdenar.disabled = true;
            btnOrdenar.innerText = "Ordenando...";
            statusMsg.innerText = ""; 

            for (let i = 0; i < barras.length; i++) {
                for (let j = 0; j < barras.length - i - 1; j++) {
                    
                    barras[j].style.backgroundColor = '#e74c3c';
                    barras[j + 1].style.backgroundColor = '#e74c3c';

                    await sleep(400);

                    const valor1 = parseInt(barras[j].getAttribute('data-valor'));
                    const valor2 = parseInt(barras[j + 1].getAttribute('data-valor'));

                    if (valor1 > valor2) {
                        container.insertBefore(barras[j + 1], barras[j]);
                        barras = document.getElementsByClassName('barra');
                    }

                    barras[j].style.backgroundColor = '#9b59b6';
                    barras[j + 1].style.backgroundColor = '#9b59b6';
                }
               
                barras[barras.length - i - 1].classList.add('ordenado');
            }
    
            if(barras.length > 0) {
                barras[0].classList.add('ordenado');
            }
            
            statusMsg.innerText = "ORDENADO! Método: Bubble Sort";
            
            btnOrdenar.disabled = false;
            btnOrdenar.innerText = "2. Ordenar!";
        }
```
---
# 🤖 Desenvolvimento Assistido

A interface visual deste projeto, incluindo o design responsivo, a estilização em CSS e a estrutura do DOM para as animações, foi desenvolvida com o auxílio do Gemini 3.0, a inteligência artificial do Google. A IA colaborou na prototipação rápida do layout e na implementação das transições visuais, permitindo um foco maior na lógica do algoritmo. Esta é uma ideia dentro deste repositório que não evidencia a minha criatividade propriamente, pois partiu do próprio gemini.

---
<div align="center">
<sub>Desenvolvido por Ector Falcão</sub>
</div>
