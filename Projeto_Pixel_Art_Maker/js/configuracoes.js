// 1. Seleção de Elementos do DOM
const canvas = document.getElementById('pixelCanvas');
const colorPicker = document.getElementById('colorPicker');
const inputHeight = document.getElementById('inputHeight');
const inputWidth = document.getElementById('inputWidth');
const btnCriar = document.getElementById('btnCriar');
const btnLimpar = document.getElementById('btnLimpar');
const btnBorracha = document.getElementById('btnBorracha');

// Variáveis de Estado
let isDrawing = false; // Verifica se o mouse está pressionado
let isErasing = false; // Verifica se o modo borracha está ativo

// 2. Função para Criar a Grade (Algoritmo de Loop)
function makeGrid() {
    // Limpa a grade anterior (se houver)
    canvas.innerHTML = '';
    
    const altura = inputHeight.value;
    const largura = inputWidth.value;

    // Configura o CSS Grid dinamicamente
    // Isso diz: "Crie X colunas de tamanho igual"
    canvas.style.gridTemplateColumns = `repeat(${largura}, 15px)`;

    // Loop Aninhado (Conceito de Algoritmos!)
    // Na verdade, para Grid HTML, um loop simples basta: Altura * Largura
    for (let i = 0; i < altura * largura; i++) {
        const pixel = document.createElement('div');
        pixel.classList.add('pixel');

        // Adiciona eventos a CADA pixel
        pixel.addEventListener('mousedown', iniciarPintura);
        pixel.addEventListener('mouseover', pintarEmMovimento);
        pixel.addEventListener('mouseup', pararPintura);
        
        // Adiciona o pixel ao canvas
        canvas.appendChild(pixel);
    }
}

// 3. Funções de Pintura
function iniciarPintura(e) {
    isDrawing = true;
    pintar(e.target); // Pinta o pixel onde clicou
}

function pintarEmMovimento(e) {
    if (isDrawing) {
        pintar(e.target); // Continua pintando se mover o mouse segurando
    }
}

function pararPintura() {
    isDrawing = false;
}

// A lógica principal de cor
function pintar(elemento) {
    if (isErasing) {
        elemento.style.backgroundColor = 'transparent'; // Borracha "limpa" a cor
    } else {
        elemento.style.backgroundColor = colorPicker.value; // Aplica a cor escolhida
    }
}

// 4. Event Listeners dos Botões
btnCriar.addEventListener('click', function(e) {
    // Evita recarregar a página
    e.preventDefault(); 
    makeGrid();
});

btnLimpar.addEventListener('click', function() {
    // Seleciona todos os pixels e remove a cor de fundo
    const pixels = document.querySelectorAll('.pixel');
    pixels.forEach(pixel => pixel.style.backgroundColor = 'transparent');
});

btnBorracha.addEventListener('click', function() {
    isErasing = !isErasing; // Alterna (True/False)
    
    // Feedback visual no botão
    if(isErasing) {
        btnBorracha.style.backgroundColor = '#e74c3c'; // Vermelho (Ativo)
        btnBorracha.textContent = 'Modo Pintar';
    } else {
        btnBorracha.style.backgroundColor = '#95a5a6'; // Cinza (Normal)
        btnBorracha.innerHTML = '<i class="material-icons">auto_fix_high</i> Borracha';
    }
});

// Para evitar bugs se soltar o mouse fora do canvas
window.addEventListener('mouseup', () => {
    isDrawing = false;
});

// Cria uma grade padrão ao iniciar
makeGrid();

const btnSalvar = document.getElementById('btnSalvar');

btnSalvar.addEventListener('click', function() {
    const elementoParaSalvar = document.getElementById('pixelCanvas');

    // 1. Guardamos os estilos originais para restaurar depois
    const bordaOriginal = elementoParaSalvar.style.border;
    const sombraOriginal = elementoParaSalvar.style.boxShadow;
    const displayOriginal = elementoParaSalvar.style.display; // Guardamos o display atual
    const marginOriginal = elementoParaSalvar.style.margin;   // Guardamos a margem

    // 2. Alterações temporárias para o "Print":
    // Removemos bordas e sombras
    elementoParaSalvar.style.border = 'none';
    elementoParaSalvar.style.boxShadow = 'none';
    // O TRUQUE: 'inline-grid' faz a div encolher para o tamanho exato dos pixels
    elementoParaSalvar.style.display = 'inline-grid'; 
    // Removemos a margem para evitar espaços brancos extras na captura
    elementoParaSalvar.style.margin = '0';

    // 3. Opção backgroundColor: null garante fundo transparente onde não tem pixel pintado
    html2canvas(elementoParaSalvar, { backgroundColor: null }).then(canvas => {
        const link = document.createElement('a');
        link.download = 'minha-pixel-art.png';
        link.href = canvas.toDataURL();
        link.click();

        // 4. Restauramos tudo para o usuário não perceber a mudança
        elementoParaSalvar.style.border = bordaOriginal;
        elementoParaSalvar.style.boxShadow = sombraOriginal;
        elementoParaSalvar.style.display = displayOriginal;
        elementoParaSalvar.style.margin = marginOriginal;
    });
});